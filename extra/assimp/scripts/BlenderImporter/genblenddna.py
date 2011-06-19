#!/usr/bin/env python3
# -*- Coding: UTF-8 -*-

# ---------------------------------------------------------------------------
# Open Asset Import Library (ASSIMP)
# ---------------------------------------------------------------------------
#
# Copyright (c) 2006-2010, ASSIMP Development Team
#
# All rights reserved.
#
# Redistribution and use of this software in source and binary forms, 
# with or without modification, are permitted provided that the following 
# conditions are met:
# 
# * Redistributions of source code must retain the above
#   copyright notice, this list of conditions and the
#   following disclaimer.
# 
# * Redistributions in binary form must reproduce the above
#   copyright notice, this list of conditions and the
#   following disclaimer in the documentation and/or other
#   materials provided with the distribution.
# 
# * Neither the name of the ASSIMP team, nor the names of its
#   contributors may be used to endorse or promote products
#   derived from this software without specific prior
#   written permission of the ASSIMP Development Team.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ---------------------------------------------------------------------------

"""Generate BlenderSceneGen.h and BlenderScene.cpp from the
data structures in BlenderScene.h to map from *any* DNA to
*our* DNA"""

import sys
import os
import re

inputfile      = os.path.join("..","..","code","BlenderScene.h")
outputfile_gen = os.path.join("..","..","code","BlenderSceneGen.h")
outputfile_src = os.path.join("..","..","code","BlenderScene.cpp")

template_gen = "BlenderSceneGen.h.template"
template_src = "BlenderScene.cpp.template"


Structure_Convert_decl =  """
template <> void Structure :: Convert<{a}> (
    {a}& dest,
    const FileDatabase& db
    ) const
"""


Structure_Convert_ptrdecl = """
    ReadFieldPtr<{policy}>({destcast}dest.{name_canonical},"{name_dna}",db);"""

Structure_Convert_arraydecl = """
    ReadFieldArray<{policy}>({destcast}dest.{name_canonical},"{name_dna}",db);"""

Structure_Convert_arraydecl2d = """
    ReadFieldArray2<{policy}>({destcast}dest.{name_canonical},"{name_dna}",db);"""

Structure_Convert_normal =  """
    ReadField<{policy}>({destcast}dest.{name_canonical},"{name_dna}",db);"""


DNA_RegisterConverters_decl = """
void DNA::RegisterConverters() """

DNA_RegisterConverters_add = """
    converters["{a}"] = DNA::FactoryPair( &Structure::Allocate<{a}>, &Structure::Convert<{a}> );"""


map_policy = {
     ""     : "ErrorPolicy_Igno"
    ,"IGNO" : "ErrorPolicy_Igno"
    ,"WARN" : "ErrorPolicy_Warn"
    ,"FAIL" : "ErrorPolicy_Fail"
}

#
def main():

    # -----------------------------------------------------------------------
    # Parse structure definitions from BlenderScene.h
    input = open(inputfile,"rt").read()

    flags = re.ASCII|re.DOTALL|re.MULTILINE
    #stripcoms = re.compile(r"/\*(.*?)*\/",flags)
    getstruct = re.compile(r"struct\s+(\w+?)\s*(:\s*ElemBase)?\s*\{(.*?)^\}\s*;",flags)
    getsmartx = re.compile(r"(std\s*::\s*)?(vector)\s*<\s*(boost\s*::\s*)?shared_(ptr)\s*<\s*(\w+)\s*>\s*>\s*",flags)
    getsmartp = re.compile(r"(boost\s*::\s*)?shared_(ptr)\s*<\s*(\w+)\s*>\s*",flags)
    getsmarta = re.compile(r"(std\s*::\s*)?(vector)\s*<\s*(\w+)\s*>\s*",flags)
    getpolicy = re.compile(r"\s*(WARN|FAIL|IGNO)",flags)
    stripenum = re.compile(r"enum\s+(\w+)\s*{.*?\}\s*;",flags)

    assert getsmartx and getsmartp and getsmarta and getpolicy and stripenum
    
    enums = set()
    #re.sub(stripcoms," ",input)
    #print(input)

    hits = {}
    while 1:
        match = re.search(getstruct,input)
        if match is None:
            break

        tmp = match.groups()[2]
        while 1:
            match2 = re.search(stripenum,tmp)
            if match2 is None:
                break
            tmp = tmp[match2.end():]
            enums.add(match2.groups()[0])

        hits[match.groups()[0]] = list(
            filter(lambda x:x[:2] != "//" and len(x),
                map(str.strip,
                    re.sub(stripenum," ",match.groups()[2]).split(";")
        )))

        input = input[match.end():]

    [print ("Enum: "+e) for e in enums]
    for k,v in hits.items():
        out = []
        for line in v:
           
            policy = "IGNO"
            py = re.search(getpolicy,line) 
            if not py is None:
                policy = py.groups()[0]
                line = re.sub(getpolicy,"",line)

            ty = re.match(getsmartx,line) or re.match(getsmartp,line)  or re.match(getsmarta,line) 
            if ty is None:
                ty = line.split(None,1)[0]
            else:
                if ty.groups()[1] == "ptr":
                    ty = ty.groups()[2] + "*"
                elif ty.groups()[1] == "vector":
                    ty = ty.groups()[-1] + ("*" if len(ty.groups()) == 3 else "**")

            #print(line)
            sp = line.split(',')
            out.append((ty,sp[0].split(None)[-1].strip(),policy))
            for m in sp[1:]:
                out.append((ty,m.strip(),policy))
                
        v[:] = out
        print("Structure {0}".format(k))
        [print("\t"+"\t".join(elem)) for elem in out]
        print("")

   
    output = open(outputfile_gen,"wt")
    templt = open(template_gen,"rt").read()
    s = ""

    # -----------------------------------------------------------------------
    # Structure::Convert<T> declarations for all supported structures
    for k,v in hits.items():
        s += Structure_Convert_decl.format(a=k)+";\n";
    output.write(templt.replace("<HERE>",s))

    output = open(outputfile_src,"wt")
    templt = open(template_src,"rt").read()
    s = ""

    # -----------------------------------------------------------------------
    # Structure::Convert<T> definitions for all supported structures
    for k,v in hits.items():
        s += "//" + "-"*80 + Structure_Convert_decl.format(a=k)+ "{ \n";

        for type, name, policy in v:
            splits = name.split("[",1)
            name_canonical = splits[0]
            #array_part = "" if len(splits)==1 else "["+splits[1]
            ptr_decl = "*"*type.count("*")
            name_dna = ptr_decl+name_canonical #+array_part

            #required  = "false"
            policy = map_policy[policy]
            destcast = "(int&)" if type in enums else ""

            # POINTER
            if ptr_decl:
               s += Structure_Convert_ptrdecl.format(**locals())
            # ARRAY MEMBER
            elif name.count('[')==1:
                s += Structure_Convert_arraydecl.format(**locals())
            elif name.count('[')==2:
                s += Structure_Convert_arraydecl2d.format(**locals())
            # NORMAL MEMBER
            else:
                s += Structure_Convert_normal.format(**locals())

        s += "\n\n\tdb.reader->IncPtr(size);\n}\n\n"


    # -----------------------------------------------------------------------
    # DNA::RegisterConverters - collect all available converter functions
    # in a std::map<name,converter_proc>
    #s += "#if 0\n"
    s += "//" + "-"*80 + DNA_RegisterConverters_decl + "{\n"
    for k,v in hits.items():
        s += DNA_RegisterConverters_add.format(a=k)
        
    s += "\n}\n"
    #s += "#endif\n"
        
    output.write(templt.replace("<HERE>",s))
        

if __name__ == "__main__":
    sys.exit(main())




    
