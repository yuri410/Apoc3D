using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;

namespace CodeCheck
{
    // Regex based c++ code checker. Style assumed.
    class Program
    {
        static Regex inheritanceMatcher =
            new Regex(@"(class|struct)(\s+\S+)?\s+(?<name>\S+)\s*:\s*(public|private|protected)\s*(?<base>[^,\s]+)[^\{\}]*\{\s*RTTI_UpcastableDerived\(\s*(?<a1>\S*)\s*\,\s*(?<a2>\S*)\s*\)");

        static Regex forLoopMatcher = new Regex(@"for\s*\(int(\w\w)?\s*(?<var>\S+)\s*=\s*\S+\s*;.*\)");

        // This detects RTTI macro usages that are not valid
        static void CheckRTTI(string fileName, string[] lines, string all)
        {
            string[] groupNames = inheritanceMatcher.GetGroupNames();

            MatchCollection matches = inheritanceMatcher.Matches(all);
            foreach (Match m in matches)
            {
                GroupCollection gc = m.Groups;

                string className = m.Groups["name"].Value;

                string baseClassName = m.Groups["base"].Value;

                string a1 = m.Groups["a1"].Value;
                string a2 = m.Groups["a2"].Value;

                if (className != a1 || baseClassName != a2)
                {
                    int lineNo = GetLineNumber(m.Index, lines);

                    if (className != a1)
                        OutputLocatableMessage(fileName, lineNo, "Invalid RTTI declaration. Class name mismatch, expected " + className + " actually " + a1);

                    if (baseClassName != a2)
                        OutputLocatableMessage(fileName, lineNo, "Invalid RTTI declaration. Base name mismatch, expected " + baseClassName + " actually " + a2);
                }
            }
        }
        
        // This detects for loops that are nested in and having the same loop variable
        // Though this is valid in c++ and shadows the previous one, it is easier to make mistakes in these scenarios.
        static void CheckFor(string fileName, string[] lines, string all)
        {
            bool[] isComment = new bool[lines.Length];
            GenerateCommentFlags(lines, isComment);

            bool[] forEnds = new bool[lines.Length];
            
            Stack<string> loopVarNames = new Stack<string>();
            Stack<int> debugList = new Stack<int>();

            int forLevel = 0;

            for (int j = 0; j < lines.Length; j++)
            {
                if (isComment[j])
                    continue;

                Match m = forLoopMatcher.Match(lines[j]);

                if (m.Success)
                {
                    string curLoopVar = m.Groups["var"].Value;
                    if (curLoopVar.Length == 0)
                        continue;

                    if (forLevel != 0 && loopVarNames.Contains(curLoopVar))
                    {
                        OutputLocatableMessage(fileName, j + 1, "Duplicated loop variable " + curLoopVar);
                    }


                    if (j < lines.Length - 1 && !lines[j + 1].Trim().StartsWith("{"))
                        continue;  // skip loop with no { body } 

                    int indent = m.Index;

                    int endLine = j + 1;
                    for (; endLine < lines.Length; endLine++)
                    {
                        string el = lines[endLine];

                        bool isClosing = el.Length > indent && el[indent] == '}';

                        if (isClosing)
                        {
                            forEnds[endLine] = true;
                            break;
                        }
                    }


                    forLevel++;
                    loopVarNames.Push(curLoopVar);
                    debugList.Push(j);
                }

                if (forEnds[j])
                {
                    forLevel--;

                    if (loopVarNames.Count > 0)
                    {
                        loopVarNames.Pop();
                        debugList.Pop();
                    }
                }
            }
        }

        static void GenerateCommentFlags(string[] lines,  bool[] isc)
        {
            bool isInComment = false;
            for (int j = 0; j < lines.Length; j++)
            {
                string tl = lines[j].Trim();

                if (tl.StartsWith(@"\\"))
                {
                    isc[j] = true;
                    continue;
                }

                
                if (tl.StartsWith(@"/*"))
                {
                    isc[j] = true;
                    isInComment = true;
                }
                if (tl.EndsWith(@"*/"))
                {
                    isc[j] = true;
                    isInComment = false;
                }
                
                if (!isc[j])
                {
                    isc[j] = isInComment;
                }
            }
        }

        static int GetLineNumber(int pos, string[] lines)
        {
            int lineNum = 0;

            for (int i = 0; i < lines.Length && pos > 0; i++)
            {
                pos -= lines[i].Length;
                lineNum = i + 1;
            }

            return lineNum;
        }

        static void OutputLocatableMessage(string filename, int lineNo, string msg)
        {
            string txt = filename + "(" + lineNo.ToString() + ") " + msg;
            Debug.WriteLine(txt);
            Console.WriteLine(txt);
        }

        delegate void Checker(string fileName, string[] lines, string all);
        static void ProcessGroup(string[] files, params Checker[] operations)
        {
            foreach (string fileName in files)
            {
                string[] content = File.ReadAllLines(fileName, Encoding.Default);

                StringBuilder allContent = new StringBuilder();

                for (int j = 0; j < content.Length; j++)
                {
                    content[j] = content[j].Replace('\t', ' ');

                    allContent.Append(content[j]);
                    allContent.Append(' ');
                }

                foreach (Checker c in operations)
                {
                    c(fileName, content, allContent.ToString());
                }
            }
        }

        static void Main(string[] args)
        {
            string srcPath;

            if (args.Length > 0)
            {
                srcPath = args[0];
            }
            else
            {
                srcPath = Path.Combine(Environment.CurrentDirectory, @"..\..\..\..\");
                srcPath = Path.GetFullPath(srcPath);
                srcPath = Path.Combine(srcPath, "Apoc3D");
            }

            string[] cppFiles = Directory.GetFiles(srcPath, "*.cpp", SearchOption.AllDirectories);
            ProcessGroup(cppFiles, CheckFor);

            string[] headerFiles = Directory.GetFiles(srcPath, "*.h", SearchOption.AllDirectories);
            ProcessGroup(headerFiles, CheckRTTI);

        }
    }
}
