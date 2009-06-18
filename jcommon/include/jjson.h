/***************************************************************************
 *   Copyright (C) 2005 by <still unknown> <modified version>              *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 a***************************************************************************/
#ifndef J_JSON_H
#define J_JSON_H

#ifdef WIN32
#define snprintf _snprintf
#define strncasecmp strnicmp
#define strcasecmp  stricmp
#endif

#include "jobject.h"

#include <stdio.h>

namespace jcommon {

class JSONNode;
class JSONPullEvent;
class JSONPullEventQueue;
class JSONReader;
class JSONObjectNode;
class JSONArrayNode;
class JSONPullParser;
class JSONArrayList;
class JSONQueue;
class JSONStringBuffer;

typedef struct tagJSONArrayListNode JSONArrayListNode_t;

class JSONArrayList {

	private:
		JSONArrayList(JSONArrayList &);
		JSONArrayList & operator=(JSONArrayList &);

		int mMaxCount;
		int mCount;
		void ** mFirst;
	
	public:
		static const int LAST_INDEX;

		JSONArrayList(int initCount = 2);
		virtual ~JSONArrayList();

		int getCount() const;
		int append(void * value);
		const void * getItem(int index) const;
		void * takeItem(int index);
		void sort( int (* cmpFunc)(const void *, const void * ));

};

class JSONQueue {

	private:
		void ** mEntries;
		unsigned int mHead;
		unsigned int mTail;
		unsigned int mCount;
		unsigned int mMaxCount;
	
	public:
		JSONQueue();
		virtual ~JSONQueue();

		void push(void *item);
		void * pop();
		void * top();

};

class JSONStringBuffer {

	private:
		JSONStringBuffer(JSONStringBuffer &);
		JSONStringBuffer & operator=(JSONStringBuffer &);

		void init();

		char * mBuffer;
		int mMaxSize;
		int mSize;	
	
	public:
		JSONStringBuffer();
		virtual ~JSONStringBuffer();
		int append(char c);
		int append(const char * value, int size = 0);
		int getSize() const;
		const char * getBuffer() const;
		char * takeBuffer();
		void clean();

};

class JSONReader {
	private:
		JSONReader( JSONReader & );

	protected:
		JSONStringBuffer * mBuffer;

		JSONReader();
		virtual ~JSONReader();

		friend class JSONPullParser;

		// help to call parser->changeReader
		void changeReader(JSONPullParser * parser, JSONReader * reader);

		// help to call parser->setError
		static void setError(JSONPullParser * parser, const char * error);

		// help to call parser->isWait4Name
		int isWait4Name( JSONPullParser * parser );

		// help to call parser->isObjectParent
		int isObjectParent(JSONPullParser * parser);

	public:
		/**
		 * @param  parser : act as reader's context
		 * @param  c : a char in json stream
		 */
		virtual void read(JSONPullParser *parser, char c) = 0;

		/**
		 * convert internal json string to event
		 * @return NULL : this reader cannot generate any event or error occured
		 */
		virtual JSONPullEvent * getEvent(JSONPullParser *parser) = 0;

		JSONReader & operator=(JSONReader &);
};

class JSONSpaceReader : public JSONReader {
	
	private:
		int mHasReadComma;
	
	public:
		JSONSpaceReader(int hasReadComma = 0);
		virtual ~JSONSpaceReader();
	
		virtual void read(JSONPullParser * parser, char c);
		virtual JSONPullEvent * getEvent(JSONPullParser * parser);

};

class JSONStartObjectReader : public JSONReader {
	
	private:
		int mHasReadLeftBrace;
	
	public:
		JSONStartObjectReader(int hasReadLeftBrace = 0);
		virtual ~JSONStartObjectReader();
	
		virtual void read(JSONPullParser * parser, char c);
		virtual JSONPullEvent * getEvent(JSONPullParser * parser);

};

class JSONEndObjectReader : public JSONReader {
	
	public:
		JSONEndObjectReader();
		virtual ~JSONEndObjectReader();
	
		virtual void read(JSONPullParser *parser, char c);
		virtual JSONPullEvent * getEvent(JSONPullParser *parser);
};

class JSONStartArrayReader : public JSONReader {

	private:
		int mHasReadLeftBracket;

	public:
		JSONStartArrayReader();
		virtual ~JSONStartArrayReader();
		
		virtual void read(JSONPullParser *parser, char c);
		virtual JSONPullEvent * getEvent(JSONPullParser * parser);

};

class JSONEndArrayReader : public JSONReader {

	public:
		JSONEndArrayReader();
		virtual ~JSONEndArrayReader();
		
		virtual void read(JSONPullParser *parser, char c);
		virtual JSONPullEvent * getEvent(JSONPullParser *parser);
};

class JSONNameReader : public JSONReader {
	
	private:
		char mLastChar;
		int mHasReadStartQuot;

	public:
		JSONNameReader();
		virtual ~JSONNameReader();
	
		virtual void read(JSONPullParser *parser, char c);
		virtual JSONPullEvent * getEvent(JSONPullParser *parser);

};

class JSONValueReader : public JSONReader {
	
	public:
		JSONValueReader();
		virtual ~JSONValueReader();
	
		virtual void read(JSONPullParser *parser, char c);
		virtual JSONPullEvent * getEvent(JSONPullParser *parser);
};

class JSONStringReader : public JSONReader {
	
	private:
		char mLastChar;
		int mHasReadStartQuot;
	
	public:
		JSONStringReader();
		virtual ~JSONStringReader();

		virtual void read(JSONPullParser *parser, char c);
		virtual JSONPullEvent * getEvent(JSONPullParser *parser);

};

class JSONNumberReader : public JSONReader {
	
	public:
		JSONNumberReader();
		virtual ~JSONNumberReader();
	
		virtual void read(JSONPullParser *parser, char c);
		virtual JSONPullEvent * getEvent(JSONPullParser *parser);
};

class JSONBooleanReader : public JSONReader {
	
	public:
		JSONBooleanReader();
		virtual ~JSONBooleanReader();
	
		virtual void read(JSONPullParser *parser, char c);
		virtual JSONPullEvent * getEvent(JSONPullParser *parser);
};

class JSONNullReader : public JSONReader {
	
	public:
		JSONNullReader();
		virtual ~JSONNullReader();
	
		virtual void read(JSONPullParser *parser, char c);
		virtual JSONPullEvent * getEvent(JSONPullParser *parser);
};

class JSONCommentReader : public JSONReader {

	private:
		int mHasReadStartSolidus;

	public:
		JSONCommentReader(JSONReader * savedReader);
		virtual ~JSONCommentReader();

		virtual void read(JSONPullParser *parser, char c);
		virtual JSONPullEvent * getEvent(JSONPullParser *parser);

		JSONReader * mSavedReader;
};

class JSONPaddingReader : public JSONReader {

	public:
		JSONPaddingReader();
		virtual ~JSONPaddingReader();

		virtual void read(JSONPullParser *parser, char c);
		virtual JSONPullEvent * getEvent(JSONPullParser *parser);
};

class JSONNode {
	protected:
		const int mType;
		JSONNode * mParent;
	
	public:
		enum { 
			eObject, 
			ePair, 
			eArray, 
			eString, 
			eDouble, 
			eInt, 
			eBoolean, 
			eNull, 
			eComment 
		};

		JSONNode( int type );
		virtual ~JSONNode();

		int getType() const;
		JSONNode * getParent() const;
		void setParent(JSONNode * parent);

		int isLastChild() const;

};

class JSONStringNode : public JSONNode {

	private:
		char * mValue;
	
	public:
		JSONStringNode(const char *value);
		virtual ~JSONStringNode();

		const char * getValue();
};

class JSONDoubleNode : public JSONNode {
	
	private:
		double mValue;
	
	public:
		JSONDoubleNode( double value );
		virtual ~JSONDoubleNode();

		double getValue();

};

class JSONIntNode : public JSONNode {
	
	private:
		int mValue;
	
	public:
		JSONIntNode(int value);
		virtual ~JSONIntNode();

		int getValue();

};

class JSONNullNode : public JSONNode {
	
	public:
		JSONNullNode();
		virtual ~JSONNullNode();
};

class JSONBooleanNode : public JSONNode {
	
	private:
		bool mValue;

	public:
		JSONBooleanNode(bool value);
		virtual ~JSONBooleanNode();

		bool getValue();
};

class JSONCommentNode : public JSONNode {
	
	private:
		char * mValue;
	
	public:
		JSONCommentNode(const char *comment);
		virtual ~JSONCommentNode();

		const char * getValue();

};

class JSONPairNode : public JSONNode {
	
	private:
		char * mName;
		JSONNode * mValue;
	
	public:
		JSONPairNode();
		virtual ~JSONPairNode();

		void setName(const char *name);
		const char * getName();

		void setValue(JSONNode *value);
		JSONNode * getValue();

};

class JSONObjectNode : public JSONNode {

	private:
		JSONArrayList * mValueList;
	
	public:
		JSONObjectNode();
		virtual ~JSONObjectNode();

		int getCount();

		int addValue(JSONPairNode *value);
		JSONPairNode * getValue(int index);
		JSONPairNode * takeValue(int index);

		int Find(const char *name);

		int isLastChild(const JSONNode *value) const;

};

class JSONArrayNode : public JSONNode {

	private:
		JSONArrayList * mValueList;
	
	public:
		JSONArrayNode();
		virtual ~JSONArrayNode();

		int getCount();

		int addValue(JSONNode *value);
		JSONNode * getValue(int index);
		JSONNode * takeValue(int index);

		int isLastChild(const JSONNode *value) const;

};

class JSONPullParser {

	protected:
		void changeReader(JSONReader *reader);

		void setError(const char *error);

		int isWait4Name();

		int isObjectParent();

		friend class JSONReader;

	private:
		JSONPullEventQueue *mEventQueue;
		JSONReader *mReader;
		JSONArrayList *mStack;

		JSONArrayList *mNameList;

		int mLevel;

		char *mError;

		char mErrorSegment[32];
		int mErrorIndex;
		int mColIndex, mRowIndex;

		int mObjectCount;
	
	public:
		JSONPullParser();
		virtual ~JSONPullParser();

		// append more input json source
		int append(const char *source, int len);

		// @return NOT NULL : the pull event
		// @return NULL : error or need more input
		JSONPullEvent * getNext();	

		// @return NOT NULL : the detail error message
		// @return NULL : no error
		const char * getError();

		int getLevel();

};

class JSONDomParser {
	
	private:
		void buildTree();
		void addNode( JSONNode * node );

		JSONPullParser * mParser;
		JSONArrayNode * mContainer;
		JSONNode * mCurrent;

	private:
		/**
		 * \brief disable these two default member functions
		 *
		 */
		JSONDomParser(JSONDomParser &);
		JSONDomParser & operator=(JSONDomParser &);
	
	public:
		JSONDomParser();
		virtual ~JSONDomParser();

		int append(const char * source, int len);
		const char * getError() const;
		const JSONNode * getValue() const;

};

class JSONDomBuffer {
	
	private:
		JSONStringBuffer * mBuffer;

		static void dump(const JSONNode * node, JSONStringBuffer * buffer, int level);
		static void dumpObject(const JSONNode * node, JSONStringBuffer * buffer, int level);
		static void dumpArray(const JSONNode * node, JSONStringBuffer * buffer, int level);
	
	public:
		JSONDomBuffer(const JSONNode * node, int indent = 1);
		virtual ~JSONDomBuffer();

		const char * getBuffer() const;
		int getSize() const;

};

class JSONPullEvent {
	
	private:
		/// Private copy constructor and copy assignment ensure classes derived from
		/// this cannot be copied.
		JSONPullEvent(JSONPullEvent &);
		JSONPullEvent & operator=(JSONPullEvent &);

	protected:
		const int mEventType;
	
	public:
		enum EventType { 
			eStartObject, 
			eEndObject, 
			eStartArray, 
			eEndArray, 
			eNull,
			eName, 
			eString, 
			eNumber, 
			eBoolean, 
			eComment, 
			ePadding 
		};

		JSONPullEvent(int eventType);
		virtual ~JSONPullEvent();

		int getEventType();

};

class JSONPullEventQueue {
	
	private:
		JSONPullEventQueue(JSONPullEventQueue &);
		JSONPullEventQueue & operator=(JSONPullEventQueue &);

		JSONQueue * mQueue;
	
	public:
		JSONPullEventQueue();
		virtual ~JSONPullEventQueue();

		void enqueue(JSONPullEvent * event);
		JSONPullEvent * dequeue();

};

class JSONStartObjectEvent : public JSONPullEvent {
	
	public:
		JSONStartObjectEvent();
		virtual ~JSONStartObjectEvent();
};

class JSONEndObjectEvent : public JSONPullEvent {
	
	public:
		JSONEndObjectEvent();
		virtual ~JSONEndObjectEvent();
};

class JSONStartArrayEvent : public JSONPullEvent {
	
	public:
		JSONStartArrayEvent();
		virtual ~JSONStartArrayEvent();
};

class JSONEndArrayEvent : public JSONPullEvent {
	
	public:
		JSONEndArrayEvent();
		virtual ~JSONEndArrayEvent();
};

class JSONTextEvent : public JSONPullEvent {

	private:
		char * mText;
	
	public:
		JSONTextEvent( int eventType );
		virtual ~JSONTextEvent();

		void setText( const char * text, int len );
		const char * getText() const;

};

class JSONNullEvent : public JSONTextEvent {
	
	public:
		JSONNullEvent();
		virtual ~JSONNullEvent();
};

class JSONNameEvent : public JSONTextEvent {
	
	public:
		JSONNameEvent();
		virtual ~JSONNameEvent();
};

class JSONStringEvent : public JSONTextEvent {
	
	public:
		JSONStringEvent();
		virtual ~JSONStringEvent();
};

class JSONNumberEvent : public JSONTextEvent {
	
	public:
		JSONNumberEvent();
		virtual ~JSONNumberEvent();
};

class JSONBooleanEvent : public JSONTextEvent {

	public:
		JSONBooleanEvent();
		virtual ~JSONBooleanEvent();
};

class JSONCommentEvent : public JSONTextEvent {

	public:
		JSONCommentEvent();
		virtual ~JSONCommentEvent();
};

class JSONPaddingEvent : public JSONTextEvent {

	public:
		JSONPaddingEvent();
		virtual ~JSONPaddingEvent();
};

class JSONCodec {

	private:
		JSONCodec();
	
	public:
		static int decode( const char * encodeValue, JSONStringBuffer * outBuffer );
		static int encode( const char * decodeValue, JSONStringBuffer * outBuffer );
		static int encode( double value, JSONStringBuffer * outBuffer );

		static const char ESCAPE_CHARS [];
		static const char UNESCAPED_CHARS [];

};

/// DFS iterator -- Depth First Searc
class JSONIterator {
	private:
		const JSONNode * mRoot;
		const JSONNode * mCurrent;

	private:
		JSONIterator( JSONIterator & );
		JSONIterator & operator=( JSONIterator & );
	
	public:
		/// node as tree node, iterator this tree by DFS
		JSONIterator( const JSONNode * node );
		virtual ~JSONIterator();

		/// @return NULL : reach the end
		const JSONNode * getNext();

};

}

#endif
