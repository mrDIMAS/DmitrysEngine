/* Copyright (c) 2017-2018 Dmitry Stepanov a.k.a mr.DIMAS
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

/********************************************/

#define DE_LINKED_LIST_DECLARE(type, name)	 \
	struct {								 \
		type* head;							 \
		type* tail;							 \
	} name

/********************************************/

#define DE_LINKED_LIST_INIT(list)			 \
	memset(&list, 0, sizeof(list))

/********************************************/

#define DE_LINKED_LIST_ITEM(type)			 \
	type* prev;								 \
	type* next

/********************************************/

#define DE_LINKED_LIST_APPEND(list, i) 	 	 \
	do { 									 \
		if(!list.head) { 					 \
			list.head = i; 				 	 \
		} 									 \
		if(list.tail) { 					 \
			i->prev = list.tail; 		 	 \
			list.tail->next = i; 		 	 \
		} 									 \
		list.tail = i; 					 	 \
	} while (0)

/********************************************/

#define DE_LINKED_LIST_REMOVE(list, i)	 	 \
	do {									 \
		if(!i) {							 \
			break;							 \
		}									 \
		if(i->next) { 					 	 \
			i->next->prev = i->prev;	 	 \
		}									 \
		if(i->prev) { 					 	 \
			i->prev->next = i->next;	 	 \
		}									 \
		if(list.head) { 					 \
			if(i == list.head) { 		 	 \
				list.head = i->next; 	 	 \
			}								 \
		}									 \
		if(list.tail) {						 \
			if(i == list.tail) {			 \
				list.tail = i->prev; 	 	 \
			} 								 \
		} 									 \
	} while (0)

/********************************************/

#define DE_LINKED_LIST_GET_AT(list, n, i) 	 \
	do {									 \
		i = NULL;						 	 \
		int _counter = 0;					 \
		void* head = list.head;				 \
		while(list.head) {					 \
			if(_counter == n) {				 \
				i = list.head;			 	 \
			}								 \
			list.head = list.head->next;	 \
			++_counter;						 \
		}									 \
		list.head = head;					 \
	} while(0) 

/********************************************/

#define DE_LINKED_LIST_REMOVE_AT(list, n)	 \
	do {									 \
		void* item;							 \
		DE_LINKED_LIST_GET_AT(list, n, item) \
		DE_LINKED_LIST_REMOVE(list, item)	 \
	} while(0)

/********************************************/

#define DE_LINKED_LIST_FOR_EACH(list, i)  	 \
	for(i = list.head; i; i = i->next)

/********************************************/

#define DE_LINKED_LIST_FOR_EACH_REV(list, i) \
	for(i = list.tail; i; i = i->prev)
