/*
WARNING: This code is modified from the original.

www.sourceforge.net/projects/tinyxml
Original code by Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#include "tinyxml.h"

#include "../IOLib/Streams.h"
#include "../IOLib/BinaryReader.h"
#include "../IOLib/BinaryWriter.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace std;

bool TiXmlBase::condenseWhiteSpace = true;


void TiXmlBase::EncodeString( const TIXML_STRING& str, TIXML_STRING* outString )
{
	int i=0;

	while( i<(int)str.length() )
	{
		unsigned char c = (unsigned char) str[i];

		if (    c == '&' 
		     && i < ( (int)str.length() - 2 )
			 && str[i+1] == '#'
			 && str[i+2] == 'x' )
		{
			// Hexadecimal character reference.
			// Pass through unchanged.
			// &#xA9;	-- copyright symbol, for example.
			//
			// The -1 is a bug fix from Rob Laveaux. It keeps
			// an overflow from happening if there is no ';'.
			// There are actually 2 ways to exit this loop -
			// while fails (error case) and break (semicolon found).
			// However, there is no mechanism (currently) for
			// this function to return an error.
			while ( i<(int)str.length()-1 )
			{
				outString->append( str.c_str() + i, 1 );
				++i;
				if ( str[i] == ';' )
					break;
			}
		}
		else if ( c == '&' )
		{
			outString->append( entity[0].str, entity[0].strLength );
			++i;
		}
		else if ( c == '<' )
		{
			outString->append( entity[1].str, entity[1].strLength );
			++i;
		}
		else if ( c == '>' )
		{
			outString->append( entity[2].str, entity[2].strLength );
			++i;
		}
		else if ( c == '\"' )
		{
			outString->append( entity[3].str, entity[3].strLength );
			++i;
		}
		else if ( c == '\'' )
		{
			outString->append( entity[4].str, entity[4].strLength );
			++i;
		}
		else if ( c < 32 )
		{
			// Easy pass at non-alpha/numeric/symbol
			// Below 32 is symbolic.
			char buf[ 32 ];
			
			#if defined(TIXML_SNPRINTF)		
				TIXML_SNPRINTF( buf, sizeof(buf), "&#x%02X;", (unsigned) ( c & 0xff ) );
			#else
				sprintf( buf, "&#x%02X;", (unsigned) ( c & 0xff ) );
			#endif		

			//*ME:	warning C4267: convert 'size_t' to 'int'
			//*ME:	Int-Cast to make compiler happy ...
			outString->append( buf, (int)strlen( buf ) );
			++i;
		}
		else
		{
			//char realc = (char) c;
			//outString->append( &realc, 1 );
			*outString += (char) c;	// somewhat more efficient function call.
			++i;
		}
	}
}



TiXmlNode::TiXmlNode( NodeType _type ) : TiXmlBase()
{
	type = _type;
}


TiXmlNode::~TiXmlNode()
{
	Clear();
}

void TiXmlNode::CopyTo( TiXmlNode* target ) const
{
	target->SetValue (value.c_str() );
	target->userData = userData; 
	target->location = location;
}

void TiXmlNode::Clear()
{
	TiXmlNode* node = firstChild;
	TiXmlNode* temp = 0;

	while ( node )
	{
		temp = node;
		node = node->next;
		delete temp;
	}

	firstChild = 0;
	lastChild = 0;
}

TiXmlNode* TiXmlNode::LinkEndChild( TiXmlNode* node )
{
	assert( node->parent == 0 || node->parent == this );
	assert( node->GetDocument() == 0 || node->GetDocument() == this->GetDocument() );

	if ( node->Type() == TiXmlNode::TINYXML_DOCUMENT )
	{
		delete node;

		if ( GetDocument() ) 
			GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
		return 0;
	}

	node->parent = this;

	node->prev = lastChild;
	node->next = 0;

	if ( lastChild )
		lastChild->next = node;
	else
		firstChild = node;			// it was an empty list.

	lastChild = node;
	return node;
}

TiXmlNode* TiXmlNode::InsertEndChild( const TiXmlNode& addThis )
{
	if ( addThis.Type() == TiXmlNode::TINYXML_DOCUMENT )
	{
		if ( GetDocument() ) 
			GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
		return 0;
	}
	TiXmlNode* node = addThis.Clone();
	if ( !node )
		return 0;

	return LinkEndChild( node );
}

TiXmlNode* TiXmlNode::InsertBeforeChild( TiXmlNode* beforeThis, const TiXmlNode& addThis )
{	
	if ( !beforeThis || beforeThis->parent != this ) {
		return 0;
	}
	if ( addThis.Type() == TiXmlNode::TINYXML_DOCUMENT )
	{
		if ( GetDocument() ) 
			GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
		return 0;
	}

	TiXmlNode* node = addThis.Clone();
	if ( !node )
		return 0;
	node->parent = this;

	node->next = beforeThis;
	node->prev = beforeThis->prev;
	if ( beforeThis->prev )
	{
		beforeThis->prev->next = node;
	}
	else
	{
		assert( firstChild == beforeThis );
		firstChild = node;
	}
	beforeThis->prev = node;
	return node;
}

TiXmlNode* TiXmlNode::InsertAfterChild( TiXmlNode* afterThis, const TiXmlNode& addThis )
{
	if ( !afterThis || afterThis->parent != this ) {
		return 0;
	}
	if ( addThis.Type() == TiXmlNode::TINYXML_DOCUMENT )
	{
		if ( GetDocument() ) 
			GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
		return 0;
	}

	TiXmlNode* node = addThis.Clone();
	if ( !node )
		return 0;
	node->parent = this;

	node->prev = afterThis;
	node->next = afterThis->next;
	if ( afterThis->next )
	{
		afterThis->next->prev = node;
	}
	else
	{
		assert( lastChild == afterThis );
		lastChild = node;
	}
	afterThis->next = node;
	return node;
}

TiXmlNode* TiXmlNode::ReplaceChild( TiXmlNode* replaceThis, const TiXmlNode& withThis )
{
	if ( !replaceThis )
		return 0;

	if ( replaceThis->parent != this )
		return 0;

	if ( withThis.ToDocument() ) {
		// A document can never be a child.	Thanks to Noam.
		TiXmlDocument* document = GetDocument();
		if ( document ) 
			document->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
		return 0;
	}

	TiXmlNode* node = withThis.Clone();
	if ( !node )
		return 0;

	node->next = replaceThis->next;
	node->prev = replaceThis->prev;

	if ( replaceThis->next )
		replaceThis->next->prev = node;
	else
		lastChild = node;

	if ( replaceThis->prev )
		replaceThis->prev->next = node;
	else
		firstChild = node;

	delete replaceThis;
	
	node->parent = this;
	return node;
}

bool TiXmlNode::RemoveChild( TiXmlNode* removeThis )
{
	if ( !removeThis ) {
		return false;
	}

	if ( removeThis->parent != this )
	{	
		assert( 0 );
		return false;
	}

	if ( removeThis->next )
		removeThis->next->prev = removeThis->prev;
	else
		lastChild = removeThis->prev;

	if ( removeThis->prev )
		removeThis->prev->next = removeThis->next;
	else
		firstChild = removeThis->next;

	delete removeThis;
	return true;
}

const TiXmlNode* TiXmlNode::FirstChild( const char * _value ) const
{
	const TiXmlNode* node;
	for ( node = firstChild; node; node = node->next )
	{
		if ( strcmp( node->Value(), _value ) == 0 )
			return node;
	}
	return 0;
}

const TiXmlNode* TiXmlNode::LastChild( const char * _value ) const
{
	const TiXmlNode* node;
	for ( node = lastChild; node; node = node->prev )
	{
		if ( strcmp( node->Value(), _value ) == 0 )
			return node;
	}
	return 0;
}

const TiXmlNode* TiXmlNode::IterateChildren( const TiXmlNode* previous ) const
{
	if ( !previous )
	{
		return FirstChild();
	}
	else
	{
		assert( previous->parent == this );
		return previous->NextSibling();
	}
}

const TiXmlNode* TiXmlNode::IterateChildren( const char * val, const TiXmlNode* previous ) const
{
	if ( !previous )
	{
		return FirstChild( val );
	}
	else
	{
		assert( previous->parent == this );
		return previous->NextSibling( val );
	}
}

const TiXmlNode* TiXmlNode::NextSibling( const char * _value ) const 
{
	const TiXmlNode* node;
	for ( node = next; node; node = node->next )
	{
		if ( strcmp( node->Value(), _value ) == 0 )
			return node;
	}
	return 0;
}

const TiXmlNode* TiXmlNode::PreviousSibling( const char * _value ) const
{
	const TiXmlNode* node;
	for ( node = prev; node; node = node->prev )
	{
		if ( strcmp( node->Value(), _value ) == 0 )
			return node;
	}
	return 0;
}

void TiXmlElement::RemoveAttribute( const char * name )
{
	TIXML_STRING str( name );
	TiXmlAttribute* node = attributeSet.Find( str );
	if ( node )
	{
		attributeSet.Remove( node );
		delete node;
	}
}

const TiXmlElement* TiXmlNode::FirstChildElement() const
{
	const TiXmlNode* node;

	for (	node = FirstChild();
			node;
			node = node->NextSibling() )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}

const TiXmlElement* TiXmlNode::FirstChildElement( const char * _value ) const
{
	const TiXmlNode* node;

	for (	node = FirstChild( _value );
			node;
			node = node->NextSibling( _value ) )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}

const TiXmlElement* TiXmlNode::NextSiblingElement() const
{
	const TiXmlNode* node;

	for (	node = NextSibling();
			node;
			node = node->NextSibling() )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}

const TiXmlElement* TiXmlNode::NextSiblingElement( const char * _value ) const
{
	const TiXmlNode* node;

	for (	node = NextSibling( _value );
			node;
			node = node->NextSibling( _value ) )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}

TiXmlHandle TiXmlNode::FirstChildElementHandle() const
{
	return TiXmlHandle(static_cast<const TiXmlNode*>(FirstChildElement()));
}
TiXmlHandle TiXmlNode::FirstChildElementHandle(const char * _value) const 
{
	return TiXmlHandle(static_cast<const TiXmlNode*>(FirstChildElement(_value)));
}
TiXmlHandle TiXmlNode::FirstChildElementHandle(const std::string& _value) const
{
	return TiXmlHandle(static_cast<const TiXmlNode*>(FirstChildElement(_value.c_str())));
}

void TiXmlNode::SearchChildElements(const std::string& _value, List<const TiXmlElement*>& results, int depth) const
{
	SearchElements([this, &_value](const TiXmlElement* elem)
	{
		if (elem == this)
			return false;

		return elem->ValueStr() == _value;
	}, results, depth);
}

List<const TiXmlElement*> TiXmlNode::SearchChildElements(const std::string& _value, int depth) const
{
	List<const TiXmlElement*> r;
	SearchChildElements(_value, r, depth);
	return r;
}

const TiXmlDocument* TiXmlNode::GetDocument() const
{
	const TiXmlNode* node;

	for( node = this; node; node = node->parent )
	{
		if ( node->ToDocument() )
			return node->ToDocument();
	}
	return 0;
}




TiXmlElement::TiXmlElement(const char * _value)
	: TiXmlNode(TiXmlNode::TINYXML_ELEMENT)
{
	value = _value;
}

TiXmlElement::TiXmlElement(const std::string& _value)
	: TiXmlNode(TiXmlNode::TINYXML_ELEMENT)
{
	value = _value;
}

TiXmlElement::TiXmlElement(const Apoc3D::String& utf16)
	: TiXmlNode(TiXmlNode::TINYXML_ELEMENT)
{
	value = Apoc3D::Utility::StringUtils::UTF16toUTF8(utf16);
}

TiXmlElement::TiXmlElement(const TiXmlElement& copy)
	: TiXmlNode(TiXmlNode::TINYXML_ELEMENT)
{
	copy.CopyTo(this);
}

TiXmlElement& TiXmlElement::operator=( const TiXmlElement& base )
{
	ClearThis();
	base.CopyTo( this );
	return *this;
}

TiXmlElement::~TiXmlElement()
{
	ClearThis();
}

void TiXmlElement::ClearThis()
{
	Clear();
	while( attributeSet.First() )
	{
		TiXmlAttribute* node = attributeSet.First();
		attributeSet.Remove( node );
		
		delete node;
	}
}

const char* TiXmlElement::Attribute( const char* name ) const
{
	const TiXmlAttribute* node = attributeSet.Find( name );
	if ( node )
		return node->Value();
	return 0;
}

const std::string* TiXmlElement::Attribute( const std::string& name ) const
{
	const TiXmlAttribute* attrib = attributeSet.Find( name );
	if ( attrib )
		return &attrib->ValueStr();
	return 0;
}

const char* TiXmlElement::Attribute( const char* name, int* i ) const
{
	const TiXmlAttribute* attrib = attributeSet.Find( name );
	const char* result = 0;

	if ( attrib ) {
		result = attrib->Value();
		if ( i ) {
			attrib->QueryIntValue( i );
		}
	}
	return result;
}

const std::string* TiXmlElement::Attribute( const std::string& name, int* i ) const
{
	const TiXmlAttribute* attrib = attributeSet.Find( name );
	const std::string* result = 0;

	if ( attrib ) {
		result = &attrib->ValueStr();
		if ( i ) {
			attrib->QueryIntValue( i );
		}
	}
	return result;
}

const char* TiXmlElement::Attribute( const char* name, double* d ) const
{
	const TiXmlAttribute* attrib = attributeSet.Find( name );
	const char* result = 0;

	if ( attrib ) {
		result = attrib->Value();
		if ( d ) {
			attrib->QueryDoubleValue( d );
		}
	}
	return result;
}

const std::string* TiXmlElement::Attribute( const std::string& name, double* d ) const
{
	const TiXmlAttribute* attrib = attributeSet.Find( name );
	const std::string* result = 0;

	if ( attrib ) {
		result = &attrib->ValueStr();
		if ( d ) {
			attrib->QueryDoubleValue( d );
		}
	}
	return result;
}

int TiXmlElement::QueryIntAttribute( const char* name, int* ival ) const
{
	const TiXmlAttribute* attrib = attributeSet.Find( name );
	if ( !attrib )
		return TIXML_NO_ATTRIBUTE;
	return attrib->QueryIntValue( ival );
}

int TiXmlElement::QueryUnsignedAttribute( const char* name, unsigned* value ) const
{
	const TiXmlAttribute* node = attributeSet.Find( name );
	if ( !node )
		return TIXML_NO_ATTRIBUTE;

	int ival = 0;
	int result = node->QueryIntValue( &ival );
	*value = (unsigned)ival;
	return result;
}

int TiXmlElement::QueryBoolAttribute( const char* name, bool* bval ) const
{
	const TiXmlAttribute* node = attributeSet.Find( name );
	if ( !node )
		return TIXML_NO_ATTRIBUTE;
	
	int result = TIXML_WRONG_TYPE;
	if (    StringEqual( node->Value(), "true", true, TIXML_ENCODING_UNKNOWN ) 
		 || StringEqual( node->Value(), "yes", true, TIXML_ENCODING_UNKNOWN ) 
		 || StringEqual( node->Value(), "1", true, TIXML_ENCODING_UNKNOWN ) ) 
	{
		*bval = true;
		result = TIXML_SUCCESS;
	}
	else if (    StringEqual( node->Value(), "false", true, TIXML_ENCODING_UNKNOWN ) 
			  || StringEqual( node->Value(), "no", true, TIXML_ENCODING_UNKNOWN ) 
			  || StringEqual( node->Value(), "0", true, TIXML_ENCODING_UNKNOWN ) ) 
	{
		*bval = false;
		result = TIXML_SUCCESS;
	}
	return result;
}

int TiXmlElement::QueryIntAttribute( const std::string& name, int* ival ) const
{
	const TiXmlAttribute* attrib = attributeSet.Find( name );
	if ( !attrib )
		return TIXML_NO_ATTRIBUTE;
	return attrib->QueryIntValue( ival );
}

int TiXmlElement::QueryDoubleAttribute( const char* name, double* dval ) const
{
	const TiXmlAttribute* attrib = attributeSet.Find( name );
	if ( !attrib )
		return TIXML_NO_ATTRIBUTE;
	return attrib->QueryDoubleValue( dval );
}

int TiXmlElement::QueryDoubleAttribute( const std::string& name, double* dval ) const
{
	const TiXmlAttribute* attrib = attributeSet.Find( name );
	if ( !attrib )
		return TIXML_NO_ATTRIBUTE;
	return attrib->QueryDoubleValue( dval );
}

void TiXmlElement::SetAttribute( const char * name, int val )
{	
	TiXmlAttribute* attrib = attributeSet.FindOrCreate( name );
	if ( attrib ) {
		attrib->SetIntValue( val );
	}
}

void TiXmlElement::SetAttribute( const std::string& name, int val )
{	
	TiXmlAttribute* attrib = attributeSet.FindOrCreate( name );
	if ( attrib ) {
		attrib->SetIntValue( val );
	}
}

void TiXmlElement::SetDoubleAttribute( const char * name, double val )
{	
	TiXmlAttribute* attrib = attributeSet.FindOrCreate( name );
	if ( attrib ) {
		attrib->SetDoubleValue( val );
	}
}

void TiXmlElement::SetDoubleAttribute( const std::string& name, double val )
{	
	TiXmlAttribute* attrib = attributeSet.FindOrCreate( name );
	if ( attrib ) {
		attrib->SetDoubleValue( val );
	}
}

void TiXmlElement::SetAttribute( const char * cname, const char * cvalue )
{
	TiXmlAttribute* attrib = attributeSet.FindOrCreate( cname );
	if ( attrib ) {
		attrib->SetValue( cvalue );
	}
}

void TiXmlElement::SetAttribute( const std::string& _name, const std::string& _value )
{
	TiXmlAttribute* attrib = attributeSet.FindOrCreate( _name );
	if ( attrib ) {
		attrib->SetValue( _value );
	}
}

void TiXmlElement::SetAttribute( const Apoc3D::String& name, const Apoc3D::String& value )
{
	std::string _name = Apoc3D::Utility::StringUtils::UTF16toUTF8(name);
	std::string _value = Apoc3D::Utility::StringUtils::UTF16toUTF8(value);
	SetAttribute(_name, _value);
}

void TiXmlElement::Print( Apoc3D::IO::BinaryWriter* bw, int depth ) const
{
	int i;
	assert( bw );
	for ( i=0; i<depth; i++ )
	{
		TIXML_STRING m1("    ");
		bw->WriteBytes(m1.c_str(), m1.size());
	}

	TIXML_STRING m2("<"); m2.append(value);
	bw->WriteBytes(m2.c_str(), m2.size());

	const TiXmlAttribute* attrib;
	for ( attrib = attributeSet.First(); attrib; attrib = attrib->Next() )
	{
		bw->WriteByte((char)' ');
		attrib->Print( bw, depth );
	}

	// There are 3 different formatting approaches:
	// 1) An element without children is printed as a <foo /> node
	// 2) An element with only a text child is printed as <foo> text </foo>
	// 3) An element with children is printed on multiple lines.
	TiXmlNode* node;
	if ( !firstChild )
	{
		TIXML_STRING m3(" />");
		bw->WriteBytes(m3.c_str(), m3.size());
	}
	else if ( firstChild == lastChild && firstChild->ToText() )
	{
		bw->WriteByte((char)'>');
		firstChild->Print( bw, depth + 1 );
		
		TIXML_STRING m3("</"); m3.append(value); m3.append(">");
		bw->WriteBytes(m3.c_str(), m3.size());
	}
	else
	{
		bw->WriteByte((char)'>');

		for ( node = firstChild; node; node=node->NextSibling() )
		{
			if ( !node->ToText() )
			{
				bw->WriteByte((char)'\n');
			}
			node->Print( bw, depth+1 );
		}
		bw->WriteByte((char)'\n');
		for( i=0; i<depth; ++i )
		{
			TIXML_STRING m1("    ");
			bw->WriteBytes(m1.c_str(), m1.size());
		}

		TIXML_STRING m3("</"); m3.append(value); m3.append(">");
		bw->WriteBytes(m3.c_str(), m3.size());
	}
}

void TiXmlElement::CopyTo( TiXmlElement* target ) const
{
	// superclass:
	TiXmlNode::CopyTo( target );

	// Element class: 
	// Clone the attributes, then clone the children.
	const TiXmlAttribute* attribute = 0;
	for(	attribute = attributeSet.First();
	attribute;
	attribute = attribute->Next() )
	{
		target->SetAttribute( attribute->Name(), attribute->Value() );
	}

	TiXmlNode* node = 0;
	for ( node = firstChild; node; node = node->NextSibling() )
	{
		target->LinkEndChild( node->Clone() );
	}
}

bool TiXmlElement::Accept( TiXmlVisitor* visitor ) const
{
	if ( visitor->VisitEnter( *this, attributeSet.First() ) ) 
	{
		for ( const TiXmlNode* node=FirstChild(); node; node=node->NextSibling() )
		{
			if ( !node->Accept( visitor ) )
				break;
		}
	}
	return visitor->VisitExit( *this );
}

TiXmlNode* TiXmlElement::Clone() const
{
	TiXmlElement* clone = new TiXmlElement(Value());
	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}

const char* TiXmlElement::GetText() const
{
	const TiXmlNode* child = this->FirstChild();
	if ( child ) {
		const TiXmlText* childText = child->ToText();
		if ( childText ) {
			return childText->Value();
		}
	}
	return 0;
}


TiXmlDocument::TiXmlDocument() : TiXmlNode( TiXmlNode::TINYXML_DOCUMENT ), selectedEncoding(TIXML_ENCODING_UNKNOWN)
{
	tabsize = 4;
	useMicrosoftBOM = false;
	ClearError();
}

TiXmlDocument::TiXmlDocument( const char * documentName ) : TiXmlNode( TiXmlNode::TINYXML_DOCUMENT ), selectedEncoding(TIXML_ENCODING_UNKNOWN)
{
	tabsize = 4;
	useMicrosoftBOM = false;
	value = documentName;
	ClearError();
}

TiXmlDocument::TiXmlDocument( const std::string& documentName ) : TiXmlNode( TiXmlNode::TINYXML_DOCUMENT ), selectedEncoding(TIXML_ENCODING_UNKNOWN)
{
	tabsize = 4;
	useMicrosoftBOM = false;
    value = documentName;
	ClearError();
}

TiXmlDocument::TiXmlDocument( const TiXmlDocument& copy ) : TiXmlNode( TiXmlNode::TINYXML_DOCUMENT ), selectedEncoding(TIXML_ENCODING_UNKNOWN)
{
	copy.CopyTo( this );
}

TiXmlDocument& TiXmlDocument::operator=( const TiXmlDocument& copy )
{
	Clear();
	copy.CopyTo( this );
	return *this;
}

//bool TiXmlDocument::SaveFile( const char * filename ) const
//{
//	// The old c stuff lives on...
//	FILE* fp = TiXmlFOpen( filename, "w" );
//	if ( fp )
//	{
//		bool result = SaveFile( fp );
//		fclose( fp );
//		return result;
//	}
//	return false;
//}
//
//
//bool TiXmlDocument::SaveFile( FILE* fp ) const
//{
//	if ( useMicrosoftBOM ) 
//	{
//		const unsigned char TIXML_UTF_LEAD_0 = 0xefU;
//		const unsigned char TIXML_UTF_LEAD_1 = 0xbbU;
//		const unsigned char TIXML_UTF_LEAD_2 = 0xbfU;
//
//		fputc( TIXML_UTF_LEAD_0, fp );
//		fputc( TIXML_UTF_LEAD_1, fp );
//		fputc( TIXML_UTF_LEAD_2, fp );
//	}
//	Print( fp, 0 );
//	return (ferror(fp) == 0);
//}

static void NormalizeLines(char* buf, int32 length)
{
	// Process the buffer in place to normalize new lines. (See comment above.)
	// Copies from the 'p' to 'q' pointer, where p can advance faster if
	// a newline-carriage return is hit.
	//
	// Wikipedia:
	// Systems based on ASCII or a compatible character set use either LF  (Line feed, '\n', 0x0A, 10 in decimal) or 
	// CR (Carriage return, '\r', 0x0D, 13 in decimal) individually, or CR followed by LF (CR+LF, 0x0D 0x0A)...
	//		* LF:    Multics, Unix and Unix-like systems (GNU/Linux, AIX, Xenix, Mac OS X, FreeBSD, etc.), BeOS, Amiga, RISC OS, and others
	//		* CR+LF: DEC RT-11 and most other early non-Unix, non-IBM OSes, CP/M, MP/M, DOS, OS/2, Microsoft Windows, Symbian OS
	//		* CR:    Commodore 8-bit machines, Apple II family, Mac OS up to version 9 and OS-9

	const char* p = buf;	// the read head
	char* q = buf;			// the write head
	const char CR = 0x0d;
	const char LF = 0x0a;

	while (*p) {
		assert(p < (buf + length));
		assert(q <= (buf + length));
		assert(q <= p);

		if (*p == CR) {
			*q++ = LF;
			p++;
			if (*p == LF) {		// check for CR+LF (and skip LF)
				p++;
			}
		}
		else {
			*q++ = *p++;
		}
	}
	assert(q <= (buf + length));
	*q = 0;
}

bool TiXmlDocument::Load(Apoc3D::IO::Stream& strm, TiXmlEncoding encoding)
{
	// Delete the existing data:
	Clear();
	location.Clear();

	int32 length = (int32)strm.getLength();
	char* buf = new char[ length+1 ];
	buf[length] = 0;

	strm.Read(buf, length);

	NormalizeLines(buf, length);

	Parse( buf, 0, encoding );

	delete [] buf;
	return !Error();
}
void TiXmlDocument::Save(Apoc3D::IO::Stream& strm)
{
	Apoc3D::IO::BinaryWriter bw(&strm, false);

	if (useMicrosoftBOM)
	{
		const unsigned char TIXML_UTF_LEAD_0 = 0xefU;
		const unsigned char TIXML_UTF_LEAD_1 = 0xbbU;
		const unsigned char TIXML_UTF_LEAD_2 = 0xbfU;

		bw.WriteByte(TIXML_UTF_LEAD_0);
		bw.WriteByte(TIXML_UTF_LEAD_1);
		bw.WriteByte(TIXML_UTF_LEAD_2);
	}
	
	Print(&bw, 0);
}

bool TiXmlDocument::Parse(const std::string& txt, TiXmlEncoding encoding)
{
	// Delete the existing data:
	Clear();
	location.Clear();

	int32 length = (int32)txt.size();
	char* buf = new char[length + 1];
	buf[length] = 0;
	memcpy(buf, txt.c_str(), length);

	NormalizeLines(buf, length);

	Parse(buf, 0, encoding);

	delete[] buf;
	return !Error();
}

void TiXmlDocument::CopyTo( TiXmlDocument* target ) const
{
	TiXmlNode::CopyTo( target );

	target->error = error;
	target->errorId = errorId;
	target->errorDesc = errorDesc;
	target->tabsize = tabsize;
	target->errorLocation = errorLocation;
	target->useMicrosoftBOM = useMicrosoftBOM;
	target->selectedEncoding = selectedEncoding;

	TiXmlNode* node = 0;
	for ( node = firstChild; node; node = node->NextSibling() )
	{
		target->LinkEndChild( node->Clone() );
	}	
}

TiXmlNode* TiXmlDocument::Clone() const
{
	TiXmlDocument* clone = new TiXmlDocument();
	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}

void TiXmlDocument::Print( Apoc3D::IO::BinaryWriter* bw, int depth ) const
{
	assert( bw );
	for ( const TiXmlNode* node=FirstChild(); node; node=node->NextSibling() )
	{
		node->Print( bw, depth );
		//fprintf( cfile, "\n" );
		bw->WriteByte((char)'\n');
	}
}

bool TiXmlDocument::Accept( TiXmlVisitor* visitor ) const
{
	if ( visitor->VisitEnter( *this ) )
	{
		for ( const TiXmlNode* node=FirstChild(); node; node=node->NextSibling() )
		{
			if ( !node->Accept( visitor ) )
				break;
		}
	}
	return visitor->VisitExit( *this );
}

Apoc3D::String TiXmlDocument::GetUTF16ElementName(const TiXmlElement* elem) const
{
	const std::string& str = elem->ValueStr();
	if (selectedEncoding == TIXML_ENCODING_LEGACY) return Apoc3D::String(str.begin(), str.end());
	return Apoc3D::Utility::StringUtils::UTF8toUTF16(str);
}
Apoc3D::String TiXmlDocument::GetUTF16ElementText(const TiXmlElement* elem) const
{
	const std::string& str = elem->GetText();
	if (selectedEncoding == TIXML_ENCODING_LEGACY) return Apoc3D::String(str.begin(), str.end());
	return Apoc3D::Utility::StringUtils::UTF8toUTF16(str);
}

Apoc3D::String TiXmlDocument::GetUTF16NodeText(const TiXmlText* text) const
{
	const std::string& str = text->ValueStr();
	if (selectedEncoding == TIXML_ENCODING_LEGACY) return Apoc3D::String(str.begin(), str.end());
	return Apoc3D::Utility::StringUtils::UTF8toUTF16(str);
}
Apoc3D::String TiXmlDocument::GetUTF16AttribName(const TiXmlAttribute* attrib) const
{
	const std::string& str = attrib->NameTStr();
	if (selectedEncoding == TIXML_ENCODING_LEGACY) return Apoc3D::String(str.begin(), str.end());
	return Apoc3D::Utility::StringUtils::UTF8toUTF16(str);
}
Apoc3D::String TiXmlDocument::GetUTF16AttribValue(const TiXmlAttribute* attrib) const
{
	const std::string& str = attrib->ValueStr();
	if (selectedEncoding == TIXML_ENCODING_LEGACY) return Apoc3D::String(str.begin(), str.end());
	return Apoc3D::Utility::StringUtils::UTF8toUTF16(str);
}
Apoc3D::String TiXmlDocument::GetUTF16AttribValue(const TiXmlElement* elem, const char* attribName) const
{
	const std::string& str = elem->Attribute(attribName);
	if (selectedEncoding == TIXML_ENCODING_LEGACY) return Apoc3D::String(str.begin(), str.end());
	return Apoc3D::Utility::StringUtils::UTF8toUTF16(str);
}

void TiXmlDocument::SetUTF16ElementName(TiXmlElement* elem, const Apoc3D::String& value) const
{
	elem->SetValue(Apoc3D::Utility::StringUtils::UTF16toUTF8(value));
}
void TiXmlDocument::SetUTF16NodeText(TiXmlText* text, const Apoc3D::String& value) const
{
	text->SetValue(Apoc3D::Utility::StringUtils::UTF16toUTF8(value));
}
void TiXmlDocument::SetUTF16AttribName(TiXmlAttribute* attrib, const Apoc3D::String& value) const
{
	attrib->SetName(Apoc3D::Utility::StringUtils::UTF16toUTF8(value));
}
void TiXmlDocument::SetUTF16AttribValue(TiXmlAttribute* attrib, const Apoc3D::String& value) const
{
	attrib->SetValue(Apoc3D::Utility::StringUtils::UTF16toUTF8(value));
}




const TiXmlAttribute* TiXmlAttribute::Next() const
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( next->value.empty() && next->name.empty() )
		return 0;
	return next;
}

/*
TiXmlAttribute* TiXmlAttribute::Next()
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( next->value.empty() && next->name.empty() )
		return 0;
	return next;
}
*/

const TiXmlAttribute* TiXmlAttribute::Previous() const
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( prev->value.empty() && prev->name.empty() )
		return 0;
	return prev;
}

/*
TiXmlAttribute* TiXmlAttribute::Previous()
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( prev->value.empty() && prev->name.empty() )
		return 0;
	return prev;
}
*/

void TiXmlAttribute::Print( Apoc3D::IO::BinaryWriter* bw, int /*depth*/, TIXML_STRING* str ) const
{
	TIXML_STRING n, v;

	EncodeString( name, &n );
	EncodeString( value, &v );

	if (value.find ('\"') == TIXML_STRING::npos) {
		if ( bw )
		{
			bw->WriteBytes(n.c_str(), n.size());
			bw->WriteByte((char)'=');
			bw->WriteByte((char)'"');
			bw->WriteBytes(v.c_str(), v.size());
			bw->WriteByte((char)'"');
		}
		if ( str ) {
			(*str) += n; (*str) += "=\""; (*str) += v; (*str) += "\"";
		}
	}
	else {
		if ( bw )
		{
			bw->WriteBytes(n.c_str(), n.size());
			bw->WriteByte((char)'=');
			bw->WriteByte((char)'"');
			bw->WriteBytes(v.c_str(), v.size());
			bw->WriteByte((char)'"');
		}
		if ( str ) {
			(*str) += n; (*str) += "='"; (*str) += v; (*str) += "'";
		}
	}
}


int TiXmlAttribute::QueryIntValue( int* ival ) const
{
	if ( TIXML_SSCANF( value.c_str(), "%d", ival ) == 1 )
		return TIXML_SUCCESS;
	return TIXML_WRONG_TYPE;
}

int TiXmlAttribute::QueryDoubleValue( double* dval ) const
{
	if ( TIXML_SSCANF( value.c_str(), "%lf", dval ) == 1 )
		return TIXML_SUCCESS;
	return TIXML_WRONG_TYPE;
}

void TiXmlAttribute::SetIntValue( int _value )
{
	char buf [64];
	#if defined(TIXML_SNPRINTF)		
		TIXML_SNPRINTF(buf, sizeof(buf), "%d", _value);
	#else
		sprintf (buf, "%d", _value);
	#endif
	SetValue (buf);
}

void TiXmlAttribute::SetDoubleValue( double _value )
{
	char buf [256];
	#if defined(TIXML_SNPRINTF)		
		TIXML_SNPRINTF( buf, sizeof(buf), "%g", _value);
	#else
		sprintf (buf, "%g", _value);
	#endif
	SetValue (buf);
}

int TiXmlAttribute::IntValue() const
{
	return atoi (value.c_str ());
}

double  TiXmlAttribute::DoubleValue() const
{
	return atof (value.c_str ());
}


TiXmlComment::TiXmlComment( const TiXmlComment& copy ) : TiXmlNode( TiXmlNode::TINYXML_COMMENT )
{
	copy.CopyTo( this );
}

TiXmlComment& TiXmlComment::operator=( const TiXmlComment& base )
{
	Clear();
	base.CopyTo( this );
	return *this;
}

void TiXmlComment::Print( Apoc3D::IO::BinaryWriter* bw, int depth ) const
{
	assert( bw );
	for ( int i=0; i<depth; i++ )
	{
		TIXML_STRING m1("    ");
		bw->WriteBytes(m1.c_str(), m1.size());
	}

	TIXML_STRING m2("<!--"); m2.append(value); m2.append("-->");
	bw->WriteBytes(m2.c_str(), m2.size());
}

void TiXmlComment::CopyTo( TiXmlComment* target ) const
{
	TiXmlNode::CopyTo( target );
}

bool TiXmlComment::Accept(TiXmlVisitor* visitor) const
{
	return visitor->Visit(*this);
}

TiXmlNode* TiXmlComment::Clone() const
{
	TiXmlComment* clone = new TiXmlComment();

	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}


TiXmlText::TiXmlText(const Apoc3D::String& utf16)
	: TiXmlNode(TiXmlNode::TINYXML_TEXT)
{
	SetValue( Apoc3D::Utility::StringUtils::UTF16toUTF8(utf16));
}

void TiXmlText::Print( Apoc3D::IO::BinaryWriter* bw, int depth ) const
{
	assert( bw );
	if ( cdata )
	{
		int i;
		bw->WriteByte((char)'\n');

		for ( i=0; i<depth; i++ )
		{
			TIXML_STRING m1("    ");
			bw->WriteBytes(m1.c_str(), m1.size());
		}

		TIXML_STRING m2("<![CDATA["); m2.append(value); m2.append("]]>\n"); 
		bw->WriteBytes(m2.c_str(), m2.size());	// unformatted output
	}
	else
	{
		TIXML_STRING buffer;
		EncodeString( value, &buffer );

		bw->WriteBytes(buffer.c_str(), buffer.size());
	}
}


void TiXmlText::CopyTo( TiXmlText* target ) const
{
	TiXmlNode::CopyTo( target );
	target->cdata = cdata;
}


bool TiXmlText::Accept( TiXmlVisitor* visitor ) const
{
	return visitor->Visit( *this );
}


TiXmlNode* TiXmlText::Clone() const
{	
	TiXmlText* clone = new TiXmlText("");

	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}


TiXmlDeclaration::TiXmlDeclaration( const char * _version,
									const char * _encoding,
									const char * _standalone )
	: TiXmlNode( TiXmlNode::TINYXML_DECLARATION )
{
	version = _version;
	encoding = _encoding;
	standalone = _standalone;
}


TiXmlDeclaration::TiXmlDeclaration(	const std::string& _version,
									const std::string& _encoding,
									const std::string& _standalone )
	: TiXmlNode( TiXmlNode::TINYXML_DECLARATION )
{
	version = _version;
	encoding = _encoding;
	standalone = _standalone;
}


TiXmlDeclaration::TiXmlDeclaration( const TiXmlDeclaration& copy )
	: TiXmlNode( TiXmlNode::TINYXML_DECLARATION )
{
	copy.CopyTo( this );	
}


TiXmlDeclaration& TiXmlDeclaration::operator=( const TiXmlDeclaration& copy )
{
	Clear();
	copy.CopyTo( this );
	return *this;
}



void TiXmlDeclaration::Print( Apoc3D::IO::BinaryWriter* bw, int /*depth*/, TIXML_STRING* str ) const
{
	if ( bw ) 
	{
		string m1("<?xml ");
		bw->WriteBytes(m1.c_str(), m1.size());
	}

	if ( str )	 (*str) += "<?xml ";

	if ( !version.empty() ) {
		if ( bw )
		{
			string m2("version=\""); m2.append(version); m2.append("\" ");
			bw->WriteBytes(m2.c_str(), m2.size());
		}

		if ( str ) 
		{
			(*str) += "version=\""; (*str) += version; (*str) += "\" "; 
		}
	}
	if ( !encoding.empty() )
	{
		if ( bw )
		{
			string m2("encoding=\""); m2.append(encoding); m2.append("\" ");
			bw->WriteBytes(m2.c_str(), m2.size());
		}
		if ( str ) { (*str) += "encoding=\""; (*str) += encoding; (*str) += "\" "; }
	}
	if ( !standalone.empty() ) 
	{
		if ( bw )
		{
			string m2("standalone=\""); m2.append(standalone); m2.append("\" ");
			bw->WriteBytes(m2.c_str(), m2.size());
		}
		if ( str ) { (*str) += "standalone=\""; (*str) += standalone; (*str) += "\" "; }
	}
	if ( bw )
	{
		string m2("?>");
		bw->WriteBytes(m2.c_str(), m2.size());
	}
	if ( str )	 (*str) += "?>";
}


void TiXmlDeclaration::CopyTo( TiXmlDeclaration* target ) const
{
	TiXmlNode::CopyTo( target );

	target->version = version;
	target->encoding = encoding;
	target->standalone = standalone;
}


bool TiXmlDeclaration::Accept( TiXmlVisitor* visitor ) const
{
	return visitor->Visit( *this );
}


TiXmlNode* TiXmlDeclaration::Clone() const
{	
	TiXmlDeclaration* clone = new TiXmlDeclaration();

	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}


void TiXmlUnknown::Print( Apoc3D::IO::BinaryWriter* bw, int depth ) const
{
	for ( int i=0; i<depth; i++ )
	{
		string m1("    ");
		bw->WriteBytes(m1.c_str(), m1.size());
	}
	bw->WriteByte((char)'<'); bw->WriteBytes(value.c_str(), value.size()); bw->WriteByte((char)'>');
}


void TiXmlUnknown::CopyTo( TiXmlUnknown* target ) const
{
	TiXmlNode::CopyTo( target );
}


bool TiXmlUnknown::Accept( TiXmlVisitor* visitor ) const
{
	return visitor->Visit( *this );
}


TiXmlNode* TiXmlUnknown::Clone() const
{
	TiXmlUnknown* clone = new TiXmlUnknown();

	if ( !clone )
		return 0;

	CopyTo( clone );
	return clone;
}


TiXmlAttributeSet::TiXmlAttributeSet()
{
	sentinel.next = &sentinel;
	sentinel.prev = &sentinel;
}


TiXmlAttributeSet::~TiXmlAttributeSet()
{
	assert( sentinel.next == &sentinel );
	assert( sentinel.prev == &sentinel );
}


void TiXmlAttributeSet::Add( TiXmlAttribute* addMe )
{
	assert( !Find( TIXML_STRING( addMe->Name() ) ) );	// Shouldn't be multiply adding to the set.


	addMe->next = &sentinel;
	addMe->prev = sentinel.prev;

	sentinel.prev->next = addMe;
	sentinel.prev      = addMe;
}

void TiXmlAttributeSet::Remove( TiXmlAttribute* removeMe )
{
	TiXmlAttribute* node;

	for( node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( node == removeMe )
		{
			node->prev->next = node->next;
			node->next->prev = node->prev;
			node->next = 0;
			node->prev = 0;
			return;
		}
	}
	assert( 0 );		// we tried to remove a non-linked attribute.
}


TiXmlAttribute* TiXmlAttributeSet::Find( const std::string& name ) const
{
	for( TiXmlAttribute* node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( node->name == name )
			return node;
	}
	return 0;
}

TiXmlAttribute* TiXmlAttributeSet::FindOrCreate( const std::string& _name )
{
	TiXmlAttribute* attrib = Find( _name );
	if (!attrib) 
	{
		attrib = new TiXmlAttribute();
		Add(attrib);
		attrib->SetName(_name);
	}
	return attrib;
}

TiXmlAttribute* TiXmlAttributeSet::Find( const char* name ) const
{
	for( TiXmlAttribute* node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( strcmp( node->name.c_str(), name ) == 0 )
			return node;
	}
	return 0;
}


TiXmlAttribute* TiXmlAttributeSet::FindOrCreate( const char* _name )
{
	TiXmlAttribute* attrib = Find( _name );
	if (!attrib) 
	{
		attrib = new TiXmlAttribute();
		Add(attrib);
		attrib->SetName(_name);
	}
	return attrib;
}


std::istream& operator>> (std::istream & in, TiXmlNode & base)
{
	TIXML_STRING tag;
	tag.reserve( 8 * 1000 );
	base.StreamIn( &in, &tag );

	TiXmlEncoding encoding = TIXML_DEFAULT_ENCODING;

	base.Parse( tag.c_str(), 0, encoding );
	return in;
}


std::ostream& operator<< (std::ostream & out, const TiXmlNode & base)
{
	TiXmlPrinter printer;
	printer.SetStreamPrinting();
	base.Accept( &printer );
	out << printer.Str();

	return out;
}


std::string& operator<< (std::string& out, const TiXmlNode& base )
{
	TiXmlPrinter printer;
	printer.SetStreamPrinting();
	base.Accept( &printer );
	out.append( printer.Str() );

	return out;
}


TiXmlHandle TiXmlHandle::FirstChild() const
{
	if ( node )
	{
		const TiXmlNode* child = node->FirstChild();
		if ( child )
			return TiXmlHandle( child );
	}
	return TiXmlHandle( 0 );
}


TiXmlHandle TiXmlHandle::FirstChild( const char * value ) const
{
	if ( node )
	{
		const TiXmlNode* child = node->FirstChild( value );
		if ( child )
			return TiXmlHandle( child );
	}
	return TiXmlHandle( 0 );
}


TiXmlHandle TiXmlHandle::FirstChildElement() const
{
	if ( node )
	{
		const TiXmlElement* child = node->FirstChildElement();
		if ( child )
			return TiXmlHandle( child );
	}
	return TiXmlHandle( 0 );
}


TiXmlHandle TiXmlHandle::FirstChildElement( const char * value ) const
{
	if ( node )
	{
		const TiXmlElement* child = node->FirstChildElement( value );
		if ( child )
			return TiXmlHandle( child );
	}
	return TiXmlHandle( 0 );
}


TiXmlHandle TiXmlHandle::Child( int count ) const
{
	if ( node )
	{
		int i;
		const TiXmlNode* child = node->FirstChild();
		for (	i=0;
				child && i<count;
				child = child->NextSibling(), ++i )
		{
			// nothing
		}
		if ( child )
			return TiXmlHandle( child );
	}
	return TiXmlHandle( 0 );
}


TiXmlHandle TiXmlHandle::Child( const char* value, int count ) const
{
	if ( node )
	{
		int i;
		const TiXmlNode* child = node->FirstChild( value );
		for (	i=0;
				child && i<count;
				child = child->NextSibling( value ), ++i )
		{
			// nothing
		}
		if ( child )
			return TiXmlHandle( child );
	}
	return TiXmlHandle( 0 );
}


TiXmlHandle TiXmlHandle::ChildElement( int count ) const
{
	if ( node )
	{
		int i;
		const TiXmlElement* child = node->FirstChildElement();
		for (	i=0;
				child && i<count;
				child = child->NextSiblingElement(), ++i )
		{
			// nothing
		}
		if ( child )
			return TiXmlHandle( child );
	}
	return TiXmlHandle( 0 );
}


TiXmlHandle TiXmlHandle::ChildElement( const char* value, int count ) const
{
	if ( node )
	{
		int i;
		const TiXmlElement* child = node->FirstChildElement( value );
		for (	i=0;
				child && i<count;
				child = child->NextSiblingElement( value ), ++i )
		{
			// nothing
		}
		if ( child )
			return TiXmlHandle( child );
	}
	return TiXmlHandle( 0 );
}

const char* TiXmlHandle::GetText() const
{
	const TiXmlElement* element = ToElement();
	if (element)
	{
		return element->GetText();
	}
	return nullptr;
}

const TiXmlElement* TiXmlHandle::ToElement() const
{
	return ((node && node->ToElement()) ? node->ToElement() : 0); 
}

const TiXmlText* TiXmlHandle::ToText() const
{
	return ((node && node->ToText()) ? node->ToText() : 0); 
}

const TiXmlUnknown* TiXmlHandle::ToUnknown() const
{
	return ((node && node->ToUnknown()) ? node->ToUnknown() : 0);
}




bool TiXmlPrinter::VisitEnter( const TiXmlDocument& )
{
	return true;
}

bool TiXmlPrinter::VisitExit( const TiXmlDocument& )
{
	return true;
}

bool TiXmlPrinter::VisitEnter( const TiXmlElement& element, const TiXmlAttribute* firstAttribute )
{
	DoIndent();
	buffer += "<";
	buffer += element.Value();

	for( const TiXmlAttribute* attrib = firstAttribute; attrib; attrib = attrib->Next() )
	{
		buffer += " ";
		attrib->Print( 0, 0, &buffer );
	}

	if ( !element.FirstChild() ) 
	{
		buffer += " />";
		DoLineBreak();
	}
	else 
	{
		buffer += ">";
		if (    element.FirstChild()->ToText()
			  && element.LastChild() == element.FirstChild()
			  && element.FirstChild()->ToText()->CDATA() == false )
		{
			simpleTextPrint = true;
			// no DoLineBreak()!
		}
		else
		{
			DoLineBreak();
		}
	}
	++depth;	
	return true;
}


bool TiXmlPrinter::VisitExit( const TiXmlElement& element )
{
	--depth;
	if ( !element.FirstChild() ) 
	{
		// nothing.
	}
	else 
	{
		if ( simpleTextPrint )
		{
			simpleTextPrint = false;
		}
		else
		{
			DoIndent();
		}
		buffer += "</";
		buffer += element.Value();
		buffer += ">";
		DoLineBreak();
	}
	return true;
}


bool TiXmlPrinter::Visit( const TiXmlText& text )
{
	if ( text.CDATA() )
	{
		DoIndent();
		buffer += "<![CDATA[";
		buffer += text.Value();
		buffer += "]]>";
		DoLineBreak();
	}
	else if ( simpleTextPrint )
	{
		TIXML_STRING str;
		TiXmlBase::EncodeString( text.ValueTStr(), &str );
		buffer += str;
	}
	else
	{
		DoIndent();
		TIXML_STRING str;
		TiXmlBase::EncodeString( text.ValueTStr(), &str );
		buffer += str;
		DoLineBreak();
	}
	return true;
}


bool TiXmlPrinter::Visit( const TiXmlDeclaration& declaration )
{
	DoIndent();
	declaration.Print( 0, 0, &buffer );
	DoLineBreak();
	return true;
}


bool TiXmlPrinter::Visit( const TiXmlComment& comment )
{
	DoIndent();
	buffer += "<!--";
	buffer += comment.Value();
	buffer += "-->";
	DoLineBreak();
	return true;
}


bool TiXmlPrinter::Visit( const TiXmlUnknown& unknown )
{
	DoIndent();
	buffer += "<";
	buffer += unknown.Value();
	buffer += ">";
	DoLineBreak();
	return true;
}

