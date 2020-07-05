# 一些备忘操作

## 数组操作

|php语法|c语法(arr是zval*)|说明|
|-|-|-|
|$arr = array() |array_init(arr)|初始化一个新数组， 还有其他方式|
|$arr[] = NULL| add_next_index_null(arr)||
|$arr[] = 42|add_next_index_long(arr, 42)||
|$arr[] = true|add_next_index_bool(arr, 1)||
|$arr[] = 3.14|add_next_index_double(arr, 3.14)||
|$arr[] = 'foo'|add_next_index_string(arr, "foo", 1)||
|$arr[] = $myvar|add_next_index_zval(arr, myvar)||
|$arr[0] = NULL|add_index_null(arr, 0)||
|$arr[1] = 42|add_index_long(arr, 1, 42)||
|$arr[2] = true|add_index_bool(arr, 2, 1)||
|$arr[3] = 3.14|add_index_double(arr, 3, 3.14)||
|$arr[5] = $myvar|add_index_zval(arr, 5, myvar)||
|$arr['abc'] = NULL|add_assoc_null(arr, "abc")||
|$arr['def'] = 711|add_assoc_long(arr, "def", 711)||
|$arr['ghi'] = true|add_assoc_bool(arr, "ghi", 1)|???|
|$arr['jkl'] = 1.44|add_assoc_double(arr, "jkl", 1.44)||
|$arr['mno'] = 'baz'|add_assoc_string(arr, "mno", "baz", 1)||
|$arr['pqr'] = $myvar|add_assoc_zval(arr, "pqr", myvar)||

参考：https://hongweipeng.com/index.php/archives/1014/