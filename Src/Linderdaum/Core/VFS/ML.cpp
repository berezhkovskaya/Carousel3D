/**
 * \file ML.cpp
 * \brief Markup language parsing library. ML node, ML node builder, String tokenizer
 * \version 0.9.1
 * \date 01/03/2011
 * \author Viktor N. Latypov, 2003-2007, 2007-2011
 * \author viktor@linderdaum.com http://www.linderdaum.com
 */

/// MLLib is used in LinderdaumEngine under exclusive license

#undef ML_HEAVY_DEBUG

#include "ML.h"

#include <cstdlib>
#include <cstring>

#ifdef ML_HEAVY_DEBUG
#include <iostream>

#define ML_TRACE(X)  std::cout << X << std::endl;

#else
#define ML_TRACE(X)
#endif

#define SET_ERROR(errCode) \
   lastLine  = parser.getLine(); \
   lastPos   = parser.getPos();  \
   lastError = errCode;



void strParser::skipString()
{
	skippedString = "";

	while ( ( pos < bufLen ) && ( buffer[pos] != 10 ) && ( buffer[pos] != 13 ) && ( buffer[pos] != 26 ) )
	{
		skippedString += buffer[pos];
		++pos;
	}
}

bool strParser::skipUntil( const LString& pattern )
{
	skippedString = "";

	int plen = static_cast<int>( pattern.length() );

	while ( buffer[pos] != 26 )
	{
		LString sstr = getSubString( pos, plen );

		if ( sstr.empty() ) { return false; }

		if ( sstr == pattern )
		{
			// we've got the pattern
			// advance in stream to skip the pattern
			pos += plen;
			curPos += plen;
			return true;
		}
		else
		{
			// no, advance further
			if ( buffer[pos] == 10 || buffer[pos] == 13 )
			{
				skippedString += "\n";
				curPos = 0;
				curLine++;
			}
			else
			{
				skippedString += buffer[pos];
				++curPos;
			}

			++pos;
		}
	}

	return false;
}

void strParser::nextToken()
{
	tokenType = 0; // NULL token

	// initial EOLN check
	if ( isEOLN() )
	{
		tokenType = T_EOLN;
		token = "\n";
		pos++;

		curPos = 0;
		curLine++;
	}

	while ( !isEOF() )
	{
		// skip spaces and EOLNs
		if ( isSpace() )
		{
			pos++;
			continue;
		}

		if ( isEOLN() )
		{
			tokenType = T_EOLN;
			token = "\n";
			pos++;

			curPos = 0;
			curLine++;
		}
		else
		{
			// not a space and not EOLN
			if ( tokenType != 0 ) { return; }

			break;
		}
	}

	// EOF check
	if ( isEOF() )
	{
		tokenType = T_EOF;
		token = "";
		return;
	}

	if ( isDelimiter( buffer[pos] ) )
	{
		token = buffer[pos];
		tokenType = T_DELIMITER;
		pos++;
		curPos++;

		// check for quoted string
		if ( token == "\"" )
		{
			getString();
			tokenType = T_QUOTED_STRING;
			return;
		}

		// check for macro/multiline string
		if ( token == "#" )
		{
			// trying to get composite token
			LString sstr = getSubString( pos, 1 );

			if ( sstr != "#" ) { return; }

			pos++; // advance and skip #
			curPos++;

			// skip until double #
			if ( skipUntil( "##" ) )
			{
				tokenType = T_MULTI_STRING;
				token = skippedString;
			}
			else
			{
				// end of file (usually unexpected, because the composite token is broken)
				tokenType = T_EOF;
				token = "";
			}

			return;
		}

		// check for single line comment
		if ( token == ";" )
		{
			//
			skipString();
			token = ";" + skippedString;
			tokenType = T_ASM_COMMENT;
		}
		else

			// check for multiline comment
			if ( token == "/" )
			{
				LString sstr = getSubString( pos, 1 );

				if ( sstr == "/" )
				{
					// it is a C++-style single line comment
					++pos;
					curPos++;

					skipString();
					token = "//" + skippedString;
					tokenType = T_CPP_COMMENT;
				}
				else
				{
					// it is a slash - nothing more
					if ( sstr != "*" ) { return; }

					// it is a beginning of multiline comment

					pos++; // advance and skip /*
					curPos++;

					if ( skipUntil( "*/" ) )
					{
						tokenType = T_MULTI_COMMENT;
						token = "/*" + skippedString + "*/";
					}
					else
					{
						// end of file
						tokenType = T_EOF;
						token = "";
					}
				}

			}

		return;
	}

	token.clear();

	if ( pos >= bufLen )
	{
		tokenType = T_EOF;
		token = "";
		return;
	}

	while ( normalChar( buffer[pos] ) )
	{
		token += buffer[pos];

		pos++;
		curPos++;
	}

	tokenType = T_STRING;
}

bool strParser::getNextChar( LString& str )
{
	if ( isEOLN() )
	{
		pos++;
		str = "\n";   // marker for EOLN

		// advance line/pos
		curPos = 0;
		curLine++;

		return false;
	}
	else if ( isEOF() )
	{
		pos++;

		str = "\n\n"; // marker for EOF
		return false;
	}
	else
	{
		str = buffer[pos];

		if ( str == "\\" )
		{
			// esc-sequence ?
			pos++;

			if ( isEOLN() )
			{
				str = "\n";   // might be a multiline macro ...
				pos++;
				// advance line/pos
				curPos = 0;
				curLine++;
				return true;
			}
			else if ( isEOF() )
			{
				pos++;
				str = "\n\n"; // EOF marker
				return false;
			}
			else
			{
				// it is an esc-sequence
				str = buffer[pos];
				pos++;
				return true;
			}
		}
		else
		{
			// usual character
			pos++;
			curPos++;
			return false;
		}
	}
}

void strParser::getString()
{
	// read the stream until "
	token = "";

	for ( ;; )
	{
		LString thisChar;
		bool isESC = getNextChar( thisChar );

		if ( !isESC )
		{
			// end of string
			if ( thisChar == "\"" ) { break; }
		}
		else
		{
			// modify thisChar
			if ( thisChar == "n" ) { thisChar = "\n"; }
		}

		token += thisChar;
	}
}

// look-ahead sliding window
LString strParser::getSubString( size_t start, size_t len )
{
	if ( start + len >= bufLen ) { return ""; }

	LString res = "";

	for ( size_t i = start ; i < start + len ; i++ ) { res += buffer[i]; }

	return res;
}



mlNode* mlTreeBuilder::build( const char* buffer, size_t bufLen )
{
	parser.setBuffer( buffer, bufLen );

	return readSection();
}

bool mlTreeBuilder::canReadUntilOpeningBracket( LString& val )
{
	val.clear();

	LString oldToken = parser.token;
	int oldTokenType = parser.tokenType;
	parser.pushPosition();

	while ( parser.tokenType != T_EOF && parser.tokenType != T_EOLN && !parser.isComment() )
	{
		if ( parser.token == "{" )
		{
			ML_TRACE( "Got bracket, can read" )

			parser.popPosition();
			return true;
		}

		val += LString( " " ) + parser.token;

		ML_TRACE( "CanRead::Processing " << parser.token )

		parser.nextToken();
	}

	parser.restorePosition();
	parser.token = oldToken;
	parser.tokenType = oldTokenType;
	return false;
}

mlNode* mlTreeBuilder::removeRedundant( mlNode* res )
{
	if ( !ASEMode ) { return res; }

	// remove redundant nodes
	if ( ( res->children.size() == 1 ) && res->getID().empty() )
	{
		ML_TRACE( "Removing redundant node" );

		mlNode* tmp = res->children[0];

		res->children.clear();
		delete res;

		return tmp;
	}

	return res;
}

// actual entry point
mlNode* mlTreeBuilder::readSection()
{
	mlNode* result = new mlNode();
	result->isSection = true;

	parser.nextToken();

	// read until the end
	while ( parser.tokenType != T_EOF )
	{
		LString nodeName  = "";
		LString nodeValue = "";
		bool isQuotedParam = false;

		//    parser.nextToken();

		ML_TRACE( "Token at the beginning : " << parser.token )

		if ( parser.tokenType == T_MACRO )
		{
			// internal preprocessing macro
			ML_TRACE( "Got macro : " << parser.token )

			return 0;
		}
		else if ( parser.token == "}" )
		{
			ML_TRACE( "finishing subsection ! " )

			break;
		}
		// New edition: allow multistrings as param names ?
		else if ( parser.tokenType == T_STRING || ( ASEMode && ( parser.tokenType == T_MULTI_STRING || parser.tokenType == T_QUOTED_STRING ) ) )
		{
			// HACK : ignore tokens beginning with "!!"
			if ( parser.token.length() > 2 )
			{
				char c1 = parser.token.at( 0 );
				char c2 = parser.token.at( 1 );

				if ( c1 == '!' && c2 == '!' )
				{
					parser.nextToken();
					continue;
				}
			}

			// new parameter node or section node
			ML_TRACE( "Reading parameter or section with name " << parser.token )

			nodeName = parser.token;
			nodeValue = "";

			mlNode* node = new mlNode();

			// gather everything that is related to this node

			bool shouldBeSubsection = false;
			bool isNonValuedNode = false;

			ML_TRACE( "reading node : " << nodeName )

			// 1. first check for embedded comments
			while ( parser.tokenType != T_EOF )
			{
				//          parser.pushPosition();
				parser.nextToken();

				if ( parser.tokenType == T_EOLN )
				{
					// this is a simple non-valued node or a subsection
					ML_TRACE( "skipping EOLN" )

					isNonValuedNode = true;
					//             parser.popPosition();
					break;
				}
				else if ( parser.isComment() )
				{
					ML_TRACE( "Embedded comment : " << parser.token )

					// if the comment is multilined then the node is multilined too
					shouldBeSubsection = parser.isMultilineStr();

					if ( parser.tokenType == T_ASM_COMMENT || parser.tokenType == T_CPP_COMMENT )
					{
						// the node is definitely multilied
						//                node->isMultiline = true;
						shouldBeSubsection = true;
					}
				}
				else
				{
					// enough of inlined comments
					ML_TRACE( "token while skipping : " << parser.token )

					//             parser.restorePosition();
					break;
				}

				//          parser.popPosition();
			}

			ML_TRACE( "after break, checking for section. Token is " << parser.token )

			// 2. then check if it is a parametrized subsection
			if ( parser.token == "(" )
			{
				ML_TRACE( "parametrized node" )

				// read parameter
				parser.nextToken();

				if ( parser.token == ")" )
				{
					ML_TRACE( "... but the param is empty" )

					// no actual parameter
				}
				else if ( parser.tokenType == T_QUOTED_STRING || parser.tokenType == T_STRING )
				{
					isQuotedParam = ( parser.tokenType == T_QUOTED_STRING );

					ML_TRACE( "got quoted string for parameter : " << parser.token )

					// read the string
					nodeValue = parser.token;

					parser.nextToken();

					if ( parser.token != ")" )
					{
						// error : EOF / comments or ?
						if ( parser.tokenType == T_EOF )
						{
							// unexpected EOF
							SET_ERROR( ML_ERR_UNEXPECTED_EOF )
						}
						else if ( parser.tokenType == T_EOLN )
						{
							// unexpected EOLN
							SET_ERROR( ML_ERR_UNEXPECTED_EOLN )
						}
						else
						{
							// expected ")"
							SET_ERROR( ML_ERR_NO_CLOSING_BRACKET );
						}

						delete result;
						return 0;
					}

					ML_TRACE( "closing bracket ) is ok" )
				}
				else
				{
					if ( parser.isComment() )
					{
						// error - embedded comments are not allowed here
						SET_ERROR( ML_ERR_NO_EMBEDDED_COMMENTS )
						delete result;
						return 0;
					}
				}

				ML_TRACE( "should be subsection, for sure" )

				shouldBeSubsection = true;

				parser.nextToken();
			}
			else
			{
				// in ASE mode sections can begin this way:
				if ( ASEMode )
				{
					ML_TRACE( "Checking ase-style section" )

					if ( canReadUntilOpeningBracket( nodeValue ) )
					{
						ML_TRACE( "OK, got node value [" << nodeValue << "]" )
						// yes, it is a paramertized section
						shouldBeSubsection = true;
					}
				}
			}

			// 3. skip embedded comments once again
			while ( parser.tokenType != T_EOF )
			{
				if ( parser.tokenType != T_EOLN )
				{
					ML_TRACE( "breaking from..." )
					break;
				}

				// just skip
				ML_TRACE( "skipping another EOLN" )

				parser.nextToken();
			}

			ML_TRACE( "token after Section param/comments/etc : |" << parser.token << "|" )

			if ( parser.token == "}" )
			{
				ML_TRACE( "Finishing subsection, name = " << nodeName )

				// finish section, but also add the node
				node->children.push_back( new mlNode/*mlParamNode*/( nodeName, "" ) );
				result->children.push_back( node );
				break;
			}

			// 4. // try to read subsection
			if ( parser.token == "{" )
			{
				ML_TRACE( "read { , switching to subsection " )

				// read subsection
				mlNode* subSection = readSection();

				if ( !subSection )
				{
					delete result;
					return 0;
				}

				if ( parser.token != "}" )
				{
					// no ending bracket ?
					SET_ERROR( ML_ERR_NO_CLOSING_BRACKET )
					delete result;
					return 0;
				}

				subSection->setValue( nodeValue );
				subSection->isQuotedParam = isQuotedParam;
				subSection->setID( nodeName );

				ML_TRACE( "Created subsection: " << nodeName << ", " << nodeValue )

				node->children.push_back( removeRedundant( subSection ) /*subSection*/ );
			}
			else
			{
				if ( shouldBeSubsection )
				{
					ML_TRACE( "expected subsection " )
					ML_TRACE( "InNode : " << node->getID() )

					// no subsection error
					SET_ERROR( ML_ERR_EXPECTED_SUBSECTION )
					delete result;
					return 0;
				}
				else
				{
					nodeValue = "";
					bool isQuotedParam = false;

					if ( !isNonValuedNode )
					{
						// collect parameters till the end of line
						while ( ( parser.tokenType != T_EOF ) && ( parser.tokenType != T_EOLN ) )
						{
							if ( parser.isString() || ( ASEMode && ( parser.tokenType == T_DELIMITER || parser.tokenType == T_NUMBER ) ) )
							{
								if ( parser.tokenType == T_QUOTED_STRING )
								{
									isQuotedParam = true;
									// nodeValue += " \""+parser.token+"\"";
								}
								else
								{
									// nodeValue += " "+parser.token;
								}

								if ( !nodeValue.empty() ) { nodeValue += " "; }

								// if IsQuotedParam then add "-" brackets around token
								nodeValue += parser.token;
							}
							else
							{
								// strange token
								// maybe, it is a parameterless node ?
								// ignore this comment by now...
								if ( !parser.isComment() )
								{
									ML_TRACE( "strange token : " << parser.token )
									ML_TRACE( "InNode : " << node->getID() )

									SET_ERROR( ML_ERR_SINGLE_STRING_EXPECTED )
									delete result;
									return 0;
								}
							}

							parser.nextToken();
						}
					}

					mlNode* paramNode = new mlNode/*mlParamNode*/( nodeName, nodeValue );
					paramNode->isQuotedParam = isQuotedParam;
					node->children.push_back( paramNode );

					if ( isNonValuedNode )
					{
						result->children.push_back( node );
						continue;
					}
				}
			}

			ML_TRACE( "Finished section, node name  = " << node->getID() << ", value = " << node->getValue() )

			result->children.push_back( removeRedundant( node ) );
		}
		else if ( parser.isComment() || ( parser.tokenType == T_EOLN ) )
		{
			// just ignore this ?
		}
		// in strict "non-ASE" mode multiline parameter names are not allowed
		else if ( parser.tokenType == T_MULTI_STRING || parser.tokenType == T_QUOTED_STRING )
		{
			ML_TRACE( "got error : SingleStringExpected" )

			// report error ?
			SET_ERROR( ML_ERR_SINGLE_STRING_EXPECTED )
			delete result;
			return 0;
		}

		parser.nextToken();
	}

	ML_TRACE( "Finished node: Name = " << result->getID() << ", Value = " << result->getValue() )
	ML_TRACE( "Children count = " << result->children.size() );

	return removeRedundant( result );
}
