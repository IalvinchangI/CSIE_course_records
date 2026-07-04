//
// Created by WavJaby on 2026/3/2.
//

#include "value_data.h"

#include <string.h>

#include "compiler_util.h"
#include "object.h"

// linkedList_init / linkedList_addp / linkedList_deleteNode / linkedList_freeA / cloneStruct 用法：見 README.md §工具函式速查
bool object_ValueDataListCreate(ObjectType valueType, const ScientificNotation* count, ValueData* valueData) {
    linkedList_init(&valueData->valueList);
    valueData->valueType = valueType;
    valueData->count = (count != NULL) ? sciToInt32(count) : 1;
    // TODO: 驗證 count > 0，負數應呼叫 yyerrorf 並回傳 true
    if (valueData->count <= 0) {
        yyerrorf("變數數量 %d 應大於零\n", valueData->count);
        return true;
    }
    return false;
}

bool object_ValueDataListAdd(ValueData* valueData, const Object* obj, const YYLTYPE* tokenLoc) {
    Object* clone = cloneStruct(Object, obj);
    // TODO: 型別相容性檢查（objValueType 與 valueData->valueType 比對）
    //       AUTO 型別應在此時確定；超過 count 上限應報錯
    ObjectType true_type = object_getValueType(clone);
    // auto type
    if (valueData->valueType == OBJECT_TYPE_AUTO) {
        valueData->valueType = true_type;
    }
    // check
    else if (valueData->valueType != true_type) {  // error
        if (
            (ObjectType_isNumber(valueData->valueType) && ObjectType_isNumber(true_type)) ||
            (valueData->valueType == OBJECT_TYPE_NUM && ObjectType_isNumber(true_type)) ||
            (ObjectType_isNumber(valueData->valueType) && true_type == OBJECT_TYPE_NUM)
        ) {
            // I32, I64, F32, F64 are NUM
        }
        else if (valueData->valueType == OBJECT_TYPE_STR) {
            // String can convert to other type when PRINT
        }
        else {
            yyerrorf("變數型別 %s 與數值 %s 型別不同\n", objectType2str[valueData->valueType], objectType2str[true_type]);
            return true;
        }
    }
    if (valueData->count <= (valueData->valueList).length) {
        yyerrorf("變數數量與宣告的 %d 不同\n", valueData->count);
        return true;
    }

    if (obj->type == OBJECT_TYPE_STR && obj->value.str)
        clone->value.str = strdup(obj->value.str);
    linkedList_addp(&valueData->valueList, 0, clone); // freeFlag=0：不自動 free，由 freeA(free) 統一釋放
    return false;
}

bool object_ValueDataListAddDefaults(ValueData* valueData, const YYLTYPE* tokenLoc) {
    // TODO: 根據 valueData->valueType，為剩餘空位（count - 已有元素數）補上各型別的零值
    //       使用對應的 object_create* 建值，再呼叫 object_ValueDataListAdd 加入
    ScientificNotation sciZero;
    chineseToArabic("零", &sciZero);
    for (int i = (valueData->valueList).length; i < valueData->count; i++) {
        Object obj;
        switch (valueData->valueType) {
            case OBJECT_TYPE_BOOL:
                obj = object_createBool(false);
                break;
            case OBJECT_TYPE_NUM:
                obj = object_createNumber(&sciZero);
                break;
            case OBJECT_TYPE_STR:
                obj = object_createStr("");
                break;
            case OBJECT_TYPE_ARRAY:
                obj = object_createArray();
                break;
            
            default:
                yyerrorf("變數型別 %s 無預設值\n", objectType2str[valueData->valueType]);
                return true;
        }
        if (object_ValueDataListAdd(valueData, &obj, tokenLoc)) {
            return true;
        }
    }
    return false;
}

Object* object_ValueDataListPop(ValueData* valueData) {
    if (valueData->valueList.length == 0)
        return NULL;
    LinkedListNode* node = valueData->valueList.head->next;
    Object* obj = node->value;
    linkedList_deleteNode(&valueData->valueList, node);
    return obj;
}

bool object_ValueDataListFree(ValueData* valueData) {
    linkedList_freeA(&valueData->valueList, free);
    return false;
}
