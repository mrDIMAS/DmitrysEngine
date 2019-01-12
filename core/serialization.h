/* Copyright (c) 2017-2019 Dmitry Stepanov a.k.a mr.DIMAS
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

/**
 * Object visitor with tree-like structure.
 * 
 * Main purpose it to make data-exchange format. Main usage is to create save files.
 * 
 */

#define DE_OBJECT_VISITOR_VERSION 1000
#define DE_OBJECT_VISITOR_MAGIC "DE:DEXF"

typedef enum de_object_visitor_data_type_t
{
	DE_OBJECT_VISITOR_DATA_TYPE_UNKNOWN,
	DE_OBJECT_VISITOR_DATA_TYPE_INT8,       /**< 1 byte */
	DE_OBJECT_VISITOR_DATA_TYPE_UINT8,      /**< 1 byte */
	DE_OBJECT_VISITOR_DATA_TYPE_INT16,      /**< 2 bytes */
	DE_OBJECT_VISITOR_DATA_TYPE_UINT16,     /**< 2 bytes */
	DE_OBJECT_VISITOR_DATA_TYPE_INT32,      /**< 4 bytes */
	DE_OBJECT_VISITOR_DATA_TYPE_UINT32,     /**< 4 bytes */
	DE_OBJECT_VISITOR_DATA_TYPE_INT64,      /**< 8 bytes */
	DE_OBJECT_VISITOR_DATA_TYPE_UINT64,     /**< 8 bytes */
	DE_OBJECT_VISITOR_DATA_TYPE_FLOAT,      /**< 4 bytes */
	DE_OBJECT_VISITOR_DATA_TYPE_DOUBLE,     /**< 8 bytes */	
	DE_OBJECT_VISITOR_DATA_TYPE_VECTOR2,    /**< 8 bytes */
	DE_OBJECT_VISITOR_DATA_TYPE_VECTOR3,    /**< 12 bytes */
	DE_OBJECT_VISITOR_DATA_TYPE_VECTOR4,    /**< 16 bytes */
	DE_OBJECT_VISITOR_DATA_TYPE_MATRIX3,    /**< 36 bytes */
	DE_OBJECT_VISITOR_DATA_TYPE_MATRIX4,    /**< 64 bytes */	
	DE_OBJECT_VISITOR_DATA_TYPE_QUATERNION, /**< 16 bytes */
	DE_OBJECT_VISITOR_DATA_TYPE_DATA,       /**< Size defined by leading field "Length", see notes below. */

	/**
	 * Where about other types? Other types are stored as nodes.
	 * 
	 * Strings:
	 * - Length (4 bytes)
	 * - Data
	 * 
	 * Pointers:
	 * - IntPtr (8 bytes)
	 * - Optional object data.
	 * 
	 * Arrays:
	 * - Length (4 bytes)
	 * - Data
	 * 
	 * Raw data:
	 * - Length (4 bytes)
	 * - Data 
	 * 
	 * */
} de_object_visitor_data_type_t;

/**
 * @brief Data field descriptor.
 */
typedef struct de_object_visitor_field_t
{
	char* name;
	uint32_t data_offset;
	uint32_t data_size;
	de_object_visitor_data_type_t data_type;
} de_object_visitor_field_t;

/**
 * @brief Object visitor node - container for data fields.
 */
typedef struct de_object_visitor_node_t
{
	char* name;
	struct de_object_visitor_node_t* parent;
	DE_ARRAY_DECLARE(de_object_visitor_field_t, fields);
	DE_ARRAY_DECLARE(struct de_object_visitor_node_t*, children);
} de_object_visitor_node_t;

typedef struct de_pointer_pair_t
{
	uint64_t last;
	void* current;
} de_pointer_pair_t;

typedef struct de_object_visitor_t
{
	char* data;
	uint32_t version;
	uint32_t data_size;
	bool is_reading;
	de_object_visitor_node_t* root;
	de_object_visitor_node_t* current_node;
	DE_ARRAY_DECLARE(de_pointer_pair_t, pointerPairs);
} de_object_visitor_t;

typedef bool(*de_visit_callback_t)(de_object_visitor_t* visitor, void* pointer);

void de_object_visitor_init(de_object_visitor_t* visitor);

void de_object_visitor_free(de_object_visitor_t* visitor);

/**
 * @brief Enters node with name 'node_name'.
 * 
 * Notes: Must be used in pair with de_object_visitor_leave_node!
 */
bool de_object_visitor_enter_node(de_object_visitor_t* visitor, const char* node_name);

/**
 * @brief Leaves current node so new current node will become parent of current node.
 * 
 * Notes: Must be used in pair with de_object_visitor_enter_node!
 */
void de_object_visitor_leave_node(de_object_visitor_t* visitor);

/**
 * @brief Loads data into object visitor from binary file.
 */
void de_object_visitor_save_binary(de_object_visitor_t* visitor, const char* file_path);

/**
 * @brief Saves object visitor data tree into binary file.
 */
void de_object_visitor_load_binary(de_object_visitor_t* visitor, const char* file_path);

/**
 * @brief Visits pointer and pointee (once).
 * @param name Name of object.
 * @param pointer_ptr Pointer to serializable pointer.
 * @param pointee_size Size of pointee in bytes.
 * @param pointee_visitor Pointer to pointee visitor function.
 * 
 * Use this functions when you have object on heap and you want to save/load it.
 * 
 * From first look it has tricky signature, but it is not too hard too use as it may look like.
 * 
 * Example:
 * 
 * some_object* foo;
 * ...
 * bool visit_foo(de_object_visitor_t* visitor, some_object* foo)
 * {
 *    // serialize internals of the object foo
 * }
 * ...
 * de_object_visitor_visit_pointer(visitor, "foo", &foo, sizeof(*foo), visit_foo);
 * 
 * So basically on loading this function will allocate memory for your object and call appropriate
 * visitor function to continue serialization.
 * 
 * Notes: Creates new node called 'name' to exclude fields name collisions. 
 * Do NOT use this method for C++ objects! Only POD types can be used here!
 */
bool de_object_visitor_visit_pointer(de_object_visitor_t* visitor, const char* name, void** pointer_ptr, size_t pointee_size, de_visit_callback_t pointee_visitor);

/**
 * @brief Useful macro which automatically calculates pointee size.
 */
#define DE_OBJECT_VISITOR_VISIT_POINTER(visitor, name, pointer_ptr, pointee_visitor) \
	de_object_visitor_visit_pointer(visitor, name, (void**)pointer_ptr, sizeof(**pointer_ptr), (de_visit_callback_t)pointee_visitor)

/**
 * @brief Visits 32-bit signed integer.
 */
bool de_object_visitor_visit_int32(de_object_visitor_t* visitor, const char* name, int32_t* integer);

/**
 * @brief Visits 16-bit signed integer.
 */
bool de_object_visitor_visit_int16(de_object_visitor_t* visitor, const char* name, int16_t* integer);

/**
 * @brief Visits 8-bit signed integer.
 */
bool de_object_visitor_visit_int8(de_object_visitor_t* visitor, const char* name, int8_t* integer);

/**
 * @brief Visits 32-bit unsigned integer.
 */
bool de_object_visitor_visit_uint32(de_object_visitor_t* visitor, const char* name, uint32_t* integer);

/**
 * @brief Visits 16-bit unsigned integer.
 */
bool de_object_visitor_visit_uint16(de_object_visitor_t* visitor, const char* name, uint16_t* integer);

/**
 * @brief Visits 8-bit unsigned integer.
 */
bool de_object_visitor_visit_uint8(de_object_visitor_t* visitor, const char* name, uint8_t* integer);

/**
 * @brief Visits float.
 */
bool de_object_visitor_visit_float(de_object_visitor_t* visitor, const char* name, float* flt);

/**
 * @brief Visits double.
 */
bool de_object_visitor_visit_double(de_object_visitor_t* visitor, const char* name, double* dbl);

/**
 * @brief Visits 2D vector.
 */
bool de_object_visitor_visit_vec2(de_object_visitor_t* visitor, const char* name, de_vec2_t* vec);

/**
 * @brief Visits 3D vector.
 */
bool de_object_visitor_visit_vec3(de_object_visitor_t* visitor, const char* name, de_vec3_t* vec);

/**
 * @brief Visits 4D vector.
 */
bool de_object_visitor_visit_vec4(de_object_visitor_t* visitor, const char* name, de_vec4_t* vec);

/**
 * @brief Visits quaternion.
 */
bool de_object_visitor_visit_quat(de_object_visitor_t* visitor, const char* name, de_quat_t* quat);

/**
 * @brief Visits 3x3 matrix.
 */
bool de_object_visitor_visit_mat3(de_object_visitor_t* visitor, const char* name, de_mat3_t* mat3);

/**
 * @brief Visits 4x4 matrix.
 */
bool de_object_visitor_visit_mat4(de_object_visitor_t* visitor, const char* name, de_mat4_t* mat4);

/**
 * @brief Visits string allocated in heap. String must be null-terminated!
 * 
 * Notes: Creates new node called 'name' to exclude fields name collisions.
 */
bool de_object_visitor_visit_heap_string(de_object_visitor_t* visitor, const char* name, char** heap_str);

/**
 * @brief Visits array. For engine arrays use macro DE_OBJECT_VISITOR_VISIT_ARRAY.
 */
bool de_object_visitor_visit_array(de_object_visitor_t* visitor, const char* name, void** array, size_t* item_count, size_t item_size, de_visit_callback_t callback);

/**
 * @brief Internal. Do not use directly, only for macro DE_OBJECT_VISITOR_VISIT_ARRAY.
 */
bool de_object_visitor_visit_array_ex(de_object_visitor_t* visitor, const char* name, void** array, size_t* item_count, size_t item_size, size_t* capacity, de_visit_callback_t callback);

/**
 * @brief Useful macro for engine array visiting which automatically calculates item size.
 */
#define DE_OBJECT_VISITOR_VISIT_ARRAY(visitor, name, array, callback) \
	de_object_visitor_visit_array_ex(visitor, name, (void**)&(array).data, &(array).size, sizeof(*(array).data), &(array)._capacity, callback)

/**
 * @brief Visits array of pointers. For engine arrays use macro DE_OBJECT_VISITOR_VISIT_POINTER_ARRAY.
 */
bool de_object_visitor_visit_pointer_array(de_object_visitor_t* visitor, const char* name, void** array, size_t* item_count, size_t pointee_size, de_visit_callback_t callback);

/**
 * @brief Internal. For engine arrays use macro DE_OBJECT_VISITOR_VISIT_POINTER_ARRAY.
 */
bool de_object_visitor_visit_pointer_array_ex(de_object_visitor_t* visitor, const char* name, void** array, size_t* item_count, size_t pointee_size, size_t* capacity, de_visit_callback_t callback);

/**
 * @brief Useful macro for visiting engine array of pointers.
 */
#define DE_OBJECT_VISITOR_VISIT_POINTER_ARRAY(visitor, name, array, callback) \
	de_object_visitor_visit_pointer_array_ex(visitor, name, (void**)&(array).data, &(array).size, sizeof(*(array).data), &(array)._capacity, (de_visit_callback_t) callback)

/**
 * @brief Serializes intrusive double linked list.
 */
bool de_object_visitor_visit_intrusive_linked_list(de_object_visitor_t* visitor, const char* name, void** head, void** tail, size_t next_offset, size_t prev_offset, size_t item_size, de_visit_callback_t callback);

/**
 * @brief Useful macro for engine intrusive double linked list serialization. 
 */
#define DE_OBJECT_VISITOR_VISIT_INTRUSIVE_LINKED_LIST(visitor, name, list, item_type, callback) \
    de_object_visitor_visit_intrusive_linked_list(visitor, name, (void**)&list.head, (void**)&list.tail, offsetof(item_type, next), offsetof(item_type, prev), sizeof(item_type), (de_visit_callback_t) callback)

/**
 * @brief Prints object visitor tree to specified stream.
 */
void de_object_visitor_print_tree(de_object_visitor_t* visitor, FILE* stream);