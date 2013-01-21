#include "array.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <assert.h>

/********************************************************
 Audio Tools, a module and set of tools for manipulating audio data
 Copyright (C) 2007-2012  Brian Langenberger

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *******************************************************/

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

struct array_i_s* array_i_new(void)
{
    int* data = malloc(sizeof(int) * 1);

    return array_i_wrap(data, 0, 1);
}

struct array_i_s* array_i_wrap(int* data, unsigned size, unsigned total_size)
{
    struct array_i_s* a = malloc(sizeof(struct array_i_s));
    a->_ = data;
    a->len = size;
    a->total_size = total_size;

    a->del = array_i_del;
    a->resize = array_i_resize;
    a->resize_for = array_i_resize_for;
    a->reset = array_i_reset;
    a->reset_for = array_i_reset_for;
    a->append = array_i_append;
    a->vappend = array_i_vappend;
    a->mappend = array_i_mappend;
    a->vset = array_i_vset;
    a->mset = array_i_mset;
    a->extend = array_i_extend;
    a->equals = array_i_equals;
    a->min = array_i_min;
    a->max = array_i_max;
    a->sum = array_i_sum;
    a->copy = array_i_copy;
    a->link = array_i_link;
    a->swap = array_i_swap;
    a->head = array_i_head;
    a->tail = array_i_tail;
    a->de_head = array_i_de_head;
    a->de_tail = array_i_de_tail;
    a->split = array_i_split;
    a->slice = array_i_slice;
    a->reverse = array_i_reverse;
    a->sort = array_i_sort;
    a->print = array_i_print;

    return a;
}

#define ARRAY_DEL(FUNC_NAME, ARRAY_TYPE) \
    void                                      \
    FUNC_NAME(ARRAY_TYPE *array)              \
    {                                         \
        free(array->_);                       \
        free(array);                          \
    }
ARRAY_DEL(array_i_del, array_i)
ARRAY_DEL(array_f_del, array_f)

#define ARRAY_RESIZE(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE) \
    void                                                          \
    FUNC_NAME(ARRAY_TYPE *array, unsigned minimum)                \
    {                                                             \
        if (minimum > array->total_size) {                         \
            array->total_size = minimum;                                \
            array->_ = realloc(array->_,                                \
                               sizeof(ARRAY_DATA_TYPE) * minimum);      \
        }                                                               \
    }
ARRAY_RESIZE(array_i_resize, array_i, int)
ARRAY_RESIZE(array_f_resize, array_f, double)
ARRAY_RESIZE(array_o_resize, array_o, void*)


#define ARRAY_RESIZE_FOR(FUNC_NAME, ARRAY_TYPE)                     \
    void                                                            \
    FUNC_NAME(ARRAY_TYPE *array, unsigned additional_items)         \
    {                                                               \
        array->resize(array, array->len + additional_items);        \
    }
ARRAY_RESIZE_FOR(array_i_resize_for, array_i);
ARRAY_RESIZE_FOR(array_f_resize_for, array_f);
ARRAY_RESIZE_FOR(array_o_resize_for, array_o);


#define ARRAY_RESET(FUNC_NAME, ARRAY_TYPE)      \
    void                                        \
    FUNC_NAME(ARRAY_TYPE *array)                \
    {                                           \
        array->len = 0;                         \
    }
ARRAY_RESET(array_i_reset, array_i)
ARRAY_RESET(array_f_reset, array_f)
ARRAY_RESET(array_li_reset, array_li)
ARRAY_RESET(array_lf_reset, array_lf)


#define ARRAY_RESET_FOR(FUNC_NAME, ARRAY_TYPE)          \
    void                                                \
    FUNC_NAME(ARRAY_TYPE *array, unsigned minimum) {    \
        array->reset(array);                            \
        array->resize(array, minimum);                  \
    }
ARRAY_RESET_FOR(array_i_reset_for, array_i);
ARRAY_RESET_FOR(array_f_reset_for, array_f);
ARRAY_RESET_FOR(array_o_reset_for, array_o);


#define ARRAY_APPEND(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE) \
    void                                                          \
    FUNC_NAME(ARRAY_TYPE *array, ARRAY_DATA_TYPE value)           \
    {                                                             \
        if (array->len == array->total_size)                      \
            array->resize(array, array->total_size * 2);          \
                                                                  \
        array->_[array->len++] = value;                          \
    }
ARRAY_APPEND(array_i_append, array_i, int)
ARRAY_APPEND(array_f_append, array_f, double)


#define ARRAY_VAPPEND(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE) \
    void                                                           \
    FUNC_NAME(ARRAY_TYPE *array, unsigned count, ...)              \
    {                                                              \
        ARRAY_DATA_TYPE i;                                         \
        va_list ap;                                                \
                                                                   \
        array->resize(array, array->len + count);                  \
        va_start(ap, count);                                       \
        for (; count > 0; count--) {                               \
            i = va_arg(ap, ARRAY_DATA_TYPE);                       \
            array->_[array->len++] = i;                            \
        }                                                          \
        va_end(ap);                                                \
    }
ARRAY_VAPPEND(array_i_vappend, array_i, int)
ARRAY_VAPPEND(array_f_vappend, array_f, double)


#define ARRAY_MAPPEND(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE) \
    void                                                                \
    FUNC_NAME(ARRAY_TYPE *array, unsigned count, ARRAY_DATA_TYPE value) \
    {                                                                   \
        array->resize(array, array->len + count);                       \
        for (; count > 0; count--) {                                    \
            array->_[array->len++] = value;                             \
        }                                                               \
    }
ARRAY_MAPPEND(array_i_mappend, array_i, int)
ARRAY_MAPPEND(array_f_mappend, array_f, double)

#define ARRAY_VSET(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE) \
    void                                                   \
    FUNC_NAME(ARRAY_TYPE *array, unsigned count, ...)      \
    {                                                               \
        va_list ap;                                                 \
                                                                    \
        array->reset_for(array, count);                             \
        va_start(ap, count);                                        \
        for (; count > 0; count--) {                                \
            const ARRAY_DATA_TYPE i = va_arg(ap, ARRAY_DATA_TYPE);  \
            array->_[array->len++] = i;                             \
        }                                                           \
        va_end(ap);                                                 \
    }
ARRAY_VSET(array_i_vset, array_i, int)
ARRAY_VSET(array_f_vset, array_f, double)


#define ARRAY_MSET(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE) \
    void                                                                \
    FUNC_NAME(ARRAY_TYPE *array, unsigned count, ARRAY_DATA_TYPE value) \
    {                                                                   \
        array->reset_for(array, count);                                 \
        for (; count > 0; count--) {                                    \
            array->_[array->len++] = value;                             \
        }                                                               \
    }
ARRAY_MSET(array_i_mset, array_i, int)
ARRAY_MSET(array_f_mset, array_f, double)

#define ARRAY_EXTEND(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE) \
    void                                                          \
    FUNC_NAME(ARRAY_TYPE *array, const ARRAY_TYPE *to_add)        \
    {                                                             \
        array->resize_for(array, to_add->len);                    \
        memcpy(array->_ + array->len,                             \
               to_add->_,                                         \
               sizeof(ARRAY_DATA_TYPE) * to_add->len);            \
        array->len += to_add->len;                                \
    }
ARRAY_EXTEND(array_i_extend, array_i, int)
ARRAY_EXTEND(array_f_extend, array_f, double)

#define ARRAY_EQUALS(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE) \
    int                                                            \
    FUNC_NAME(const ARRAY_TYPE *array, const ARRAY_TYPE *compare)  \
    {                                                              \
        assert(array->_ != NULL);                                  \
        assert(compare->_ != NULL);                                \
        if (array->len == compare->len) {                               \
            return (memcmp(array->_, compare->_,                        \
                           sizeof(ARRAY_DATA_TYPE) * array->len) == 0); \
        } else                                                          \
            return 0;                                                   \
    }
ARRAY_EQUALS(array_i_equals, array_i, int)
ARRAY_EQUALS(array_f_equals, array_f, double)
ARRAY_EQUALS(array_li_equals, array_li, int)
ARRAY_EQUALS(array_lf_equals, array_lf, double)


#define ARRAY_I_MIN(FUNC_NAME, ARRAY_TYPE) \
    int                                    \
    FUNC_NAME(const ARRAY_TYPE *array)     \
    {                                      \
        int min = INT_MAX;                 \
        unsigned i;                        \
                                           \
        assert(array->_ != NULL);          \
        for (i = 0; i < array->len; i++)   \
            if (array->_[i] < min)         \
                min = array->_[i];         \
                                           \
        return min;                        \
    }
ARRAY_I_MIN(array_i_min, array_i)
ARRAY_I_MIN(array_li_min, array_li)

#define ARRAY_I_MAX(FUNC_NAME, ARRAY_TYPE)      \
    int                                          \
    FUNC_NAME(const ARRAY_TYPE *array)           \
    {                                            \
        int max = INT_MIN;                       \
        unsigned i;                              \
                                                 \
        assert(array->_ != NULL);                \
        for (i = 0; i < array->len; i++)         \
            if (array->_[i] > max)               \
                max = array->_[i];               \
                                                 \
        return max;                              \
    }
ARRAY_I_MAX(array_i_max, array_i)
ARRAY_I_MAX(array_li_max, array_li)

#define ARRAY_I_SUM(FUNC_NAME, ARRAY_TYPE) \
    int                                         \
    FUNC_NAME(const ARRAY_TYPE *array)          \
    {                                           \
        int accumulator = 0;                    \
        const int *data = array->_;             \
        unsigned size = array->len;             \
        unsigned i;                             \
                                                \
        assert(array->_ != NULL);               \
        for (i = 0; i < size; i++)              \
            accumulator += data[i];             \
                                                \
        return accumulator;                     \
    }
ARRAY_I_SUM(array_i_sum, array_i)
ARRAY_I_SUM(array_li_sum, array_li)

#define ARRAY_COPY(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE) \
    void                                                        \
    FUNC_NAME(const ARRAY_TYPE *array, ARRAY_TYPE *copy)        \
    {                                                               \
        if (array != copy) {                                        \
            copy->resize(copy, array->len);                         \
            memcpy(copy->_, array->_,                               \
                   array->len * sizeof(ARRAY_DATA_TYPE));           \
            copy->len = array->len;                                 \
        }                                                           \
    }
ARRAY_COPY(array_i_copy, array_i, int)
ARRAY_COPY(array_f_copy, array_f, double)

#define ARRAY_L_COPY(FUNC_NAME, ARRAY_TYPE, TARGET_TYPE, ARRAY_DATA_TYPE) \
    void                                                                \
    FUNC_NAME(const ARRAY_TYPE *array, TARGET_TYPE *copy)               \
    {                                                                   \
        copy->resize(copy, array->len);                                 \
        memcpy(copy->_, array->_,                                       \
               array->len * sizeof(ARRAY_DATA_TYPE));                   \
        copy->len = array->len;                                         \
    }
ARRAY_L_COPY(array_li_copy, array_li, array_i, int)
ARRAY_L_COPY(array_lf_copy, array_lf, array_f, double)


#define ARRAY_LINK(FUNC_NAME, ARRAY_TYPE, ARRAY_LINK_TYPE) \
    void                                                        \
    FUNC_NAME(const ARRAY_TYPE *array, ARRAY_LINK_TYPE *link)   \
    {                                                           \
        link->_ = array->_;                                     \
        link->len = array->len;                                 \
    }
ARRAY_LINK(array_i_link, array_i, array_li)
ARRAY_LINK(array_f_link, array_f, array_lf)
ARRAY_LINK(array_li_link, array_li, array_li)
ARRAY_LINK(array_lf_link, array_lf, array_lf)


#define ARRAY_SWAP(FUNC_NAME, ARRAY_TYPE)                  \
    void                                                        \
    FUNC_NAME(ARRAY_TYPE *array, ARRAY_TYPE *swap)              \
    {                                                           \
        ARRAY_TYPE temp;                                        \
        temp._ = array->_;                                      \
        temp.len = array->len;                                  \
        temp.total_size = array->total_size;                    \
        array->_ = swap->_;                                     \
        array->len = swap->len;                                 \
        array->total_size = swap->total_size;                   \
        swap->_ = temp._;                                       \
        swap->len = temp.len;                                   \
        swap->total_size = temp.total_size;                     \
    }
ARRAY_SWAP(array_i_swap, array_i)
ARRAY_SWAP(array_f_swap, array_f)
ARRAY_SWAP(array_o_swap, array_o)
ARRAY_SWAP(array_ia_swap, array_ia)
ARRAY_SWAP(array_fa_swap, array_fa)
ARRAY_SWAP(array_lia_swap, array_lia)
ARRAY_SWAP(array_lfa_swap, array_lfa)
ARRAY_SWAP(array_iaa_swap, array_iaa)
ARRAY_SWAP(array_faa_swap, array_faa)


#define ARRAY_HEAD(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE)     \
    void                                                            \
    FUNC_NAME(const ARRAY_TYPE *array, unsigned count, ARRAY_TYPE *head) \
    {                                                                   \
        const unsigned to_copy = MIN(count, array->len);                \
                                                                        \
        if (head != array) {                                            \
            head->resize(head, to_copy);                                \
            memcpy(head->_, array->_,                                   \
                   sizeof(ARRAY_DATA_TYPE) * to_copy);                  \
            head->len = to_copy;                                        \
        } else {                                                        \
            head->len = to_copy;                                        \
        }                                                               \
    }
ARRAY_HEAD(array_i_head, array_i, int)
ARRAY_HEAD(array_f_head, array_f, double)

#define ARRAY_DE_HEAD(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE)        \
    void                                                             \
    FUNC_NAME(const ARRAY_TYPE *array, unsigned count, ARRAY_TYPE *tail) \
    {                                                                   \
        unsigned to_copy;                                               \
        count = MIN(count, array->len);                                 \
        to_copy = array->len - count;                                   \
                                                                        \
        if (tail != array) {                                            \
            tail->resize(tail, to_copy);                                \
            memcpy(tail->_, array->_ + count,                           \
                   sizeof(ARRAY_DATA_TYPE) * to_copy);                  \
            tail->len = to_copy;                                        \
        } else {                                                        \
            memmove(tail->_, array->_ + count,                          \
                    sizeof(ARRAY_DATA_TYPE) * to_copy);                 \
            tail->len = to_copy;                                        \
        }                                                               \
    }
ARRAY_DE_HEAD(array_i_de_head, array_i, int)
ARRAY_DE_HEAD(array_f_de_head, array_f, double)

#define ARRAY_TAIL(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE)     \
    void                                                                \
    FUNC_NAME(const ARRAY_TYPE *array, unsigned count, ARRAY_TYPE *tail) \
    {                                                                   \
        const unsigned to_copy = MIN(count, array->len);                \
                                                                        \
        if (tail != array) {                                            \
            tail->resize(tail, to_copy);                                \
            memcpy(tail->_, array->_ + (array->len - to_copy),          \
                   sizeof(ARRAY_DATA_TYPE) * to_copy);                  \
            tail->len = to_copy;                                        \
        } else {                                                        \
            memmove(tail->_, array->_ + (array->len - to_copy),         \
                    sizeof(ARRAY_DATA_TYPE) * to_copy);                 \
            tail->len = to_copy;                                        \
        }                                                               \
    }
ARRAY_TAIL(array_i_tail, array_i, int)
ARRAY_TAIL(array_f_tail, array_f, double)

#define ARRAY_DE_TAIL(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE)   \
    void                                                             \
    FUNC_NAME(const ARRAY_TYPE *array, unsigned count, ARRAY_TYPE *head) \
    {                                                                   \
        unsigned to_copy;                                               \
        count = MAX(count, array->len);                                 \
        to_copy = array->len - count;                                   \
                                                                        \
        if (head != array) {                                            \
            head->resize(head, to_copy);                                \
            memcpy(head->_, array->_,                                   \
                   sizeof(ARRAY_DATA_TYPE) * to_copy);                  \
            head->len = to_copy;                                        \
        } else {                                                        \
            head->len = to_copy;                                        \
        }                                                               \
    }
ARRAY_DE_TAIL(array_i_de_tail, array_i, int)
ARRAY_DE_TAIL(array_f_de_tail, array_f, double)

#define ARRAY_SPLIT(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE)             \
    void                                                                \
    FUNC_NAME(const ARRAY_TYPE *array, unsigned count,                  \
              ARRAY_TYPE *head, ARRAY_TYPE *tail)                       \
    {                                                                   \
        /*ensure we don't try to move too many items*/                  \
        const unsigned to_head = MIN(count, array->len);                \
        const unsigned to_tail = array->len - to_head;                  \
                                                                        \
        if ((head == array) && (tail == array)) {                       \
            /*do nothing*/                                              \
            return;                                                     \
        } else if (head == tail) {                                      \
            /*copy all data to head*/                                   \
            array->copy(array, head);                                   \
        } else if ((head != array) && (tail == array)) {                \
            /*move "count" values to head and shift the rest down*/     \
            head->resize(head, to_head);                                \
            memcpy(head->_, array->_,                                   \
                   sizeof(ARRAY_DATA_TYPE) * to_head);                  \
            head->len = to_head;                                        \
                                                                        \
            memmove(tail->_, array->_ + to_head,                        \
                    sizeof(ARRAY_DATA_TYPE) * to_tail);                 \
            tail->len = to_tail;                                        \
        } else if ((head == array) && (tail != array)) {                \
            /*move "count" values from our end to tail and reduce our size*/ \
            tail->resize(tail, to_tail);                                \
            memcpy(tail->_, array->_ + to_head,                         \
                   sizeof(ARRAY_DATA_TYPE) * to_tail);                  \
            tail->len = to_tail;                                        \
                                                                        \
            head->len = to_head;                                        \
        } else {                                                        \
            /*copy "count" values to "head" and the remainder to "tail"*/ \
            head->resize(head, to_head);                                \
            memcpy(head->_, array->_,                                   \
                   sizeof(ARRAY_DATA_TYPE) * to_head);                  \
            head->len = to_head;                                        \
                                                                        \
            tail->resize(tail, to_tail);                                \
            memcpy(tail->_, array->_ + to_head,                         \
                   sizeof(ARRAY_DATA_TYPE) * to_tail);                  \
            tail->len = to_tail;                                        \
        }                                                               \
    }
ARRAY_SPLIT(array_i_split, array_i, int)
ARRAY_SPLIT(array_f_split, array_f, double)

#define ARRAY_SLICE(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE, ARRAY_NEW)  \
    void                                                                \
    FUNC_NAME(const ARRAY_TYPE *array,                                  \
              unsigned start, unsigned end, unsigned jump,              \
              ARRAY_TYPE *slice)                                        \
    {                                                                   \
        ARRAY_TYPE* temp;                                               \
        assert(start <= end);                                           \
        assert(jump > 0);                                               \
                                                                        \
        start = MIN(start, array->len);                                 \
        end = MIN(end, array->len);                                     \
                                                                        \
        if (array != slice) {                                           \
            if (jump == 1) {                                            \
                slice->resize(slice, end - start);                      \
                memcpy(slice->_, array->_ + start,                      \
                       sizeof(ARRAY_DATA_TYPE) * (end - start));        \
                slice->len = end - start;                               \
            } else {                                                    \
                slice->reset(slice);                                    \
                for (; start < end; start += jump)                      \
                    slice->append(slice, array->_[start]);              \
            }                                                           \
        } else {                                                        \
            if (jump == 1) {                                            \
                memmove(slice->_, array->_ + start,                     \
                        sizeof(ARRAY_DATA_TYPE) * (end - start));       \
                slice->len = end - start;                               \
            } else {                                                    \
                temp = ARRAY_NEW();                                     \
                for (; start < end; start += jump)                      \
                    temp->append(temp, array->_[start]);                \
                temp->copy(temp, slice);                                \
                temp->del(temp);                                        \
            }                                                           \
        }                                                               \
    }
ARRAY_SLICE(array_i_slice, array_i, int, array_i_new)
ARRAY_SLICE(array_f_slice, array_f, double, array_f_new)

#define ARRAY_REVERSE(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE)   \
    void                                                        \
    FUNC_NAME(ARRAY_TYPE *array)                                \
    {                                                           \
        unsigned i;                                             \
        unsigned j;                                             \
        ARRAY_DATA_TYPE *data = array->_;                       \
                                                                \
        if (array->len > 0) {                                   \
            for (i = 0, j = array->len - 1; i < j; i++, j--) {  \
                ARRAY_DATA_TYPE x = data[i];                    \
                data[i] = data[j];                              \
                data[j] = x;                                    \
            }                                                   \
        }                                                       \
    }
ARRAY_REVERSE(array_i_reverse, array_i, int)
ARRAY_REVERSE(array_f_reverse, array_f, double)
ARRAY_REVERSE(array_ia_reverse, array_ia, array_i*);
ARRAY_REVERSE(array_fa_reverse, array_fa, array_f*);
ARRAY_REVERSE(array_lia_reverse, array_lia, array_li*);
ARRAY_REVERSE(array_lfa_reverse, array_lfa, array_lf*);
ARRAY_REVERSE(array_iaa_reverse, array_iaa, array_ia*);
ARRAY_REVERSE(array_faa_reverse, array_faa, array_fa*);

int array_int_cmp(const void *x, const void *y)
{
    return *(int*)x - *(int*)y;
}

void array_i_sort(array_i *array)
{
    qsort(array->_, (size_t)(array->len), sizeof(int), array_int_cmp);
}

#define ARRAY_I_PRINT(FUNC_NAME, ARRAY_TYPE)            \
    void                                                \
    FUNC_NAME(const ARRAY_TYPE *array, FILE* output)    \
    {                                                   \
        unsigned i;                                     \
                                                        \
        putc('[', output);                              \
        if (array->len == 1) {                          \
            fprintf(output, "%d", array->_[0]);         \
        } else if (array->len > 1) {                    \
            for (i = 0; i < array->len - 1; i++)        \
                fprintf(output, "%d, ", array->_[i]);   \
            fprintf(output, "%d", array->_[i]);         \
        }                                               \
        putc(']', output);                              \
    }
ARRAY_I_PRINT(array_i_print, array_i)
ARRAY_I_PRINT(array_li_print, array_li)

struct array_li_s* array_li_new(void)
{
    struct array_li_s* array = malloc(sizeof(struct array_li_s));
    array->_ = NULL;
    array->len = 0;

    array->del = array_li_del;
    array->reset = array_li_reset;
    array->equals = array_li_equals;
    array->min = array_li_min;
    array->max = array_li_max;
    array->sum = array_li_sum;
    array->copy = array_li_copy;
    array->link = array_li_link;
    array->swap = array_li_swap;
    array->head = array_li_head;
    array->tail = array_li_tail;
    array->de_head = array_li_de_head;
    array->de_tail = array_li_de_tail;
    array->split = array_li_split;
    array->print = array_li_print;

    return array;
}

#define ARRAY_L_DEL(FUNC_NAME, ARRAY_TYPE)      \
    void                                        \
    FUNC_NAME(ARRAY_TYPE *array)                \
    {                                           \
        free(array);                            \
    }
ARRAY_L_DEL(array_li_del, array_li)
ARRAY_L_DEL(array_lf_del, array_lf)

#define ARRAY_L_SWAP(FUNC_NAME, ARRAY_TYPE)         \
    void                                            \
    FUNC_NAME(ARRAY_TYPE *array, ARRAY_TYPE *swap)  \
    {                                               \
        ARRAY_TYPE temp;                            \
        temp._ = array->_;                          \
        temp.len = array->len;                      \
        array->_ = swap->_;                         \
        array->len = swap->len;                     \
        swap->_ = temp._;                           \
        swap->len = temp.len;                       \
    }
ARRAY_L_SWAP(array_li_swap, array_li)
ARRAY_L_SWAP(array_lf_swap, array_lf)


#define ARRAY_L_HEAD(FUNC_NAME, ARRAY_TYPE)                             \
    void                                                                \
    FUNC_NAME(const ARRAY_TYPE *array, unsigned count, ARRAY_TYPE *head) \
    {                                                                   \
        const unsigned to_copy = MIN(count, array->len);                \
        assert(array->_ != NULL);                                       \
        head->_ = array->_;                                             \
        head->len = to_copy;                                            \
    }
ARRAY_L_HEAD(array_li_head, array_li)
ARRAY_L_HEAD(array_lf_head, array_lf)

#define ARRAY_L_DE_HEAD(FUNC_NAME, ARRAY_TYPE)                          \
    void                                                                \
    FUNC_NAME(const ARRAY_TYPE *array, unsigned count, ARRAY_TYPE *tail) \
    {                                                                   \
        unsigned to_copy;                                               \
        assert(array->_ != NULL);                                       \
        count = MIN(count, array->len);                                 \
        to_copy = array->len - count;                                   \
                                                                        \
        tail->_ = array->_ + count;                                     \
        tail->len = to_copy;                                            \
    }
ARRAY_L_DE_HEAD(array_li_de_head, array_li)
ARRAY_L_DE_HEAD(array_lf_de_head, array_lf)

#define ARRAY_L_TAIL(FUNC_NAME, ARRAY_TYPE)                             \
    void                                                                \
    FUNC_NAME(const ARRAY_TYPE *array, unsigned count, ARRAY_TYPE *tail) \
    {                                                                   \
        const unsigned to_copy = MIN(count, array->len);                \
        assert(array->_ != NULL);                                       \
        tail->_ = array->_ + (array->len - to_copy);                    \
        tail->len = to_copy;                                            \
    }
ARRAY_L_TAIL(array_li_tail, array_li)
ARRAY_L_TAIL(array_lf_tail, array_lf)

#define ARRAY_L_DE_TAIL(FUNC_NAME, ARRAY_TYPE)                          \
    void                                                                \
    FUNC_NAME(const ARRAY_TYPE *array, unsigned count, ARRAY_TYPE *head) \
    {                                                                   \
        assert(array->_ != NULL);                                       \
        head->len = array->len - MAX(count, array->len);                \
    }
ARRAY_L_DE_TAIL(array_li_de_tail, array_li)
ARRAY_L_DE_TAIL(array_lf_de_tail, array_lf)

#define ARRAY_L_SPLIT(FUNC_NAME, ARRAY_TYPE)             \
    void                                                 \
    FUNC_NAME(const ARRAY_TYPE *array, unsigned count,   \
              ARRAY_TYPE *head, ARRAY_TYPE *tail)        \
    {                                                    \
        /*ensure we don't try to move too many items*/   \
        const unsigned to_head = MIN(count, array->len); \
        const unsigned to_tail = array->len - to_head;   \
        assert(array->_ != NULL);                        \
                                                         \
        if ((head == array) && (tail == array)) {        \
            /*do nothing*/                               \
            return;                                      \
        } else if (head == tail) {                       \
            /*copy all data to head*/                    \
            head->_ = array->_;                          \
            head->len = array->len;                      \
        } else {                                         \
            head->_ = array->_;                          \
            head->len = to_head;                         \
            tail->_ = array->_ + to_head;                \
            tail->len = to_tail;                         \
        }                                                \
    }
ARRAY_L_SPLIT(array_li_split, array_li)
ARRAY_L_SPLIT(array_lf_split, array_lf)

array_f* array_f_new(void)
{
    double* data = malloc(sizeof(double) * 1);

    return array_f_wrap(data, 0, 1);
}


array_f* array_f_wrap(double* data, unsigned size, unsigned total_size)
{
    array_f* a = malloc(sizeof(array_f));
    a->_ = data;
    a->len = size;
    a->total_size = total_size;

    a->del = array_f_del;
    a->resize = array_f_resize;
    a->resize_for = array_f_resize_for;
    a->reset = array_f_reset;
    a->reset_for = array_f_reset_for;
    a->append = array_f_append;
    a->vappend = array_f_vappend;
    a->mappend = array_f_mappend;
    a->vset = array_f_vset;
    a->mset = array_f_mset;
    a->extend = array_f_extend;
    a->equals = array_f_equals;
    a->min = array_f_min;
    a->max = array_f_max;
    a->sum = array_f_sum;
    a->copy = array_f_copy;
    a->link = array_f_link;
    a->swap = array_f_swap;
    a->head = array_f_head;
    a->tail = array_f_tail;
    a->de_head = array_f_de_head;
    a->de_tail = array_f_de_tail;
    a->split = array_f_split;
    a->slice = array_f_slice;
    a->reverse = array_f_reverse;
    a->sort = array_f_sort;
    a->print = array_f_print;

    return a;
}

#define ARRAY_F_MIN(FUNC_NAME, ARRAY_TYPE)      \
    double                                      \
    FUNC_NAME(const ARRAY_TYPE *array)          \
    {                                           \
        double min = DBL_MAX;                   \
        unsigned i;                             \
                                                \
        assert(array->_ != NULL);               \
        for (i = 0; i < array->len; i++)        \
            if (array->_[i] < min)              \
                min = array->_[i];              \
                                                \
        return min;                             \
    }
ARRAY_F_MIN(array_f_min, array_f)
ARRAY_F_MIN(array_lf_min, array_lf)

#define ARRAY_F_MAX(FUNC_NAME, ARRAY_TYPE)      \
    double                                      \
    FUNC_NAME(const ARRAY_TYPE *array)          \
    {                                           \
        double max = DBL_MIN;                   \
        unsigned i;                             \
                                                \
        assert(array->_ != NULL);               \
        for (i = 0; i < array->len; i++)        \
            if (array->_[i] > max)              \
                max = array->_[i];              \
                                                \
        return max;                             \
    }
ARRAY_F_MAX(array_f_max, array_f)
ARRAY_F_MAX(array_lf_max, array_lf)

#define ARRAY_F_SUM(FUNC_NAME, ARRAY_TYPE)      \
    double                                      \
    FUNC_NAME(const ARRAY_TYPE *array)          \
    {                                           \
        double accumulator = 0.0;               \
        const double *data = array->_;          \
        unsigned size = array->len;             \
        unsigned i;                             \
                                                \
        assert(array->_ != NULL);               \
        for (i = 0; i < size; i++)              \
            accumulator += data[i];             \
                                                \
        return accumulator;                     \
    }
ARRAY_F_SUM(array_f_sum, array_f)
ARRAY_F_SUM(array_lf_sum, array_lf)

int array_float_cmp(const void *x, const void *y)
{
    if (*(double*)x < *(double*)y)
        return -1;
    else if (*(double*)x == *(double*)y)
        return 0;
    else
        return 1;
}

void array_f_sort(array_f *array)
{
    qsort(array->_, (size_t)(array->len), sizeof(double), array_float_cmp);
}

#define ARRAY_F_PRINT(FUNC_NAME, ARRAY_TYPE)            \
    void                                                \
    FUNC_NAME(const ARRAY_TYPE *array, FILE* output)    \
    {                                                   \
        unsigned i;                                     \
                                                        \
        putc('[', output);                              \
        if (array->len == 1) {                          \
            fprintf(output, "%f", array->_[0]);         \
        } else if (array->len > 1) {                    \
            for (i = 0; i < array->len - 1; i++)        \
                fprintf(output, "%f, ", array->_[i]);   \
            fprintf(output, "%f", array->_[i]);         \
        }                                               \
        putc(']', output);                              \
    }
ARRAY_F_PRINT(array_f_print, array_f)
ARRAY_F_PRINT(array_lf_print, array_lf)

struct array_lf_s* array_lf_new(void)
{
    struct array_lf_s* array = malloc(sizeof(struct array_lf_s));
    array->_ = NULL;
    array->len = 0;

    array->del = array_lf_del;
    array->reset = array_lf_reset;
    array->equals = array_lf_equals;
    array->min = array_lf_min;
    array->max = array_lf_max;
    array->sum = array_lf_sum;
    array->copy = array_lf_copy;
    array->link = array_lf_link;
    array->swap = array_lf_swap;
    array->head = array_lf_head;
    array->tail = array_lf_tail;
    array->de_head = array_lf_de_head;
    array->de_tail = array_lf_de_tail;
    array->split = array_lf_split;
    array->print = array_lf_print;

    return array;
}


struct array_ia_s*
array_ia_new(void)
{
    struct array_ia_s* a = malloc(sizeof(struct array_ia_s));
    unsigned i;

    a->_ = malloc(sizeof(struct array_i_s*) * 1);
    a->len = 0;
    a->total_size = 1;

    for (i = 0; i < 1; i++) {
        a->_[i] = array_i_new();
    }

    a->del = array_ia_del;
    a->resize = array_ia_resize;
    a->reset = array_ia_reset;
    a->append = array_ia_append;
    a->extend = array_ia_extend;
    a->equals = array_ia_equals;
    a->copy = array_ia_copy;
    a->swap = array_ia_swap;
    a->zip = array_ia_zip;
    a->split = array_ia_split;
    a->cross_split = array_ia_cross_split;
    a->reverse = array_ia_reverse;
    a->print = array_ia_print;

    return a;
}

struct array_lia_s*
array_lia_new(void)
{
    struct array_lia_s* a = malloc(sizeof(struct array_lia_s));
    unsigned i;

    a->_ = malloc(sizeof(struct array_li_s*) * 1);
    a->len = 0;
    a->total_size = 1;

    for (i = 0; i < 1; i++) {
        a->_[i] = array_li_new();
    }

    a->del = array_lia_del;
    a->resize = array_lia_resize;
    a->reset = array_lia_reset;
    a->append = array_lia_append;
    a->extend = array_lia_extend;
    a->equals = array_lia_equals;
    a->copy = array_lia_copy;
    a->swap = array_lia_swap;
    a->split = array_lia_split;
    a->cross_split = array_lia_cross_split;
    a->reverse = array_lia_reverse;
    a->print = array_lia_print;

    return a;
}

struct array_lfa_s*
array_lfa_new(void)
{
    struct array_lfa_s* a = malloc(sizeof(struct array_lfa_s));
    unsigned i;

    a->_ = malloc(sizeof(struct array_lf_s*) * 1);
    a->len = 0;
    a->total_size = 1;

    for (i = 0; i < 1; i++) {
        a->_[i] = array_lf_new();
    }

    a->del = array_lfa_del;
    a->resize = array_lfa_resize;
    a->reset = array_lfa_reset;
    a->append = array_lfa_append;
    a->extend = array_lfa_extend;
    a->equals = array_lfa_equals;
    a->copy = array_lfa_copy;
    a->swap = array_lfa_swap;
    a->split = array_lfa_split;
    a->cross_split = array_lfa_cross_split;
    a->reverse = array_lfa_reverse;
    a->print = array_lfa_print;

    return a;
}

#define ARRAY_A_DEL(FUNC_NAME, ARRAY_TYPE)      \
    void                                        \
    FUNC_NAME(ARRAY_TYPE *array)                \
    {                                           \
        unsigned i;                             \
                                                \
        for (i = 0; i < array->total_size; i++) \
            array->_[i]->del(array->_[i]);      \
                                                \
        free(array->_);                         \
        free(array);                            \
    }
ARRAY_A_DEL(array_ia_del, array_ia)
ARRAY_A_DEL(array_fa_del, array_fa)
ARRAY_A_DEL(array_lia_del, array_lia)
ARRAY_A_DEL(array_lfa_del, array_lfa)
ARRAY_A_DEL(array_iaa_del, array_iaa)
ARRAY_A_DEL(array_faa_del, array_faa)

#define ARRAY_A_RESIZE(FUNC_NAME, ARRAY_TYPE, SUB_ARRAY_TYPE, NEW_FUNC) \
    void                                                                \
    FUNC_NAME(ARRAY_TYPE *array, unsigned minimum)                      \
    {                                                                   \
        if (minimum > array->total_size) {                              \
            array->_ = realloc(array->_,                                \
                               sizeof(SUB_ARRAY_TYPE*) * minimum);      \
            while (array->total_size < minimum) {                       \
                array->_[array->total_size++] = NEW_FUNC();             \
            }                                                           \
        }                                                               \
    }
ARRAY_A_RESIZE(array_ia_resize, array_ia, array_i, array_i_new)
ARRAY_A_RESIZE(array_fa_resize, array_fa, array_f, array_f_new)
ARRAY_A_RESIZE(array_lia_resize, array_lia, array_li, array_li_new)
ARRAY_A_RESIZE(array_lfa_resize, array_lfa, array_lf, array_lf_new)
ARRAY_A_RESIZE(array_iaa_resize, array_iaa, array_ia, array_ia_new)
ARRAY_A_RESIZE(array_faa_resize, array_faa, array_fa, array_fa_new)

#define ARRAY_A_RESET(FUNC_NAME, ARRAY_TYPE)    \
    void                                        \
    FUNC_NAME(ARRAY_TYPE *array)                \
    {                                           \
        unsigned i;                             \
        for (i = 0; i < array->total_size; i++) \
            array->_[i]->reset(array->_[i]);    \
        array->len = 0;                         \
    }
ARRAY_A_RESET(array_ia_reset, array_ia)
ARRAY_A_RESET(array_fa_reset, array_fa)
ARRAY_A_RESET(array_lia_reset, array_lia)
ARRAY_A_RESET(array_lfa_reset, array_lfa)
ARRAY_A_RESET(array_iaa_reset, array_iaa)
ARRAY_A_RESET(array_faa_reset, array_faa)


/*note that this does *not* reset the appended sub-array
  prior to returning it

  the array_?a_resize, array_?a_reset and array_?a_split functions
  should always ensure that newly added sub-arrays are reset*/
#define ARRAY_A_APPEND(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE)  \
    ARRAY_DATA_TYPE*                                            \
    FUNC_NAME(ARRAY_TYPE *array)                                \
    {                                                           \
        if (array->len == array->total_size)                    \
            array->resize(array, array->total_size * 2);        \
                                                                \
        return array->_[array->len++];                          \
    }
ARRAY_A_APPEND(array_ia_append, array_ia, array_i)
ARRAY_A_APPEND(array_fa_append, array_fa, array_f)
ARRAY_A_APPEND(array_lia_append, array_lia, array_li)
ARRAY_A_APPEND(array_lfa_append, array_lfa, array_lf)
ARRAY_A_APPEND(array_iaa_append, array_iaa, array_ia)
ARRAY_A_APPEND(array_faa_append, array_faa, array_fa)

#define ARRAY_A_EXTEND(FUNC_NAME, ARRAY_TYPE, TRANSFER)             \
    void                                                            \
    FUNC_NAME(ARRAY_TYPE *array, const ARRAY_TYPE *to_add)          \
    {                                                               \
        unsigned i;                                                 \
        const unsigned len = to_add->len;                               \
        for (i = 0; i < len; i++) {                                     \
            to_add->_[i]->TRANSFER(to_add->_[i], array->append(array)); \
        }                                                               \
    }
ARRAY_A_EXTEND(array_ia_extend, array_ia, copy)
ARRAY_A_EXTEND(array_fa_extend, array_fa, copy)
ARRAY_A_EXTEND(array_lia_extend, array_lia, link)
ARRAY_A_EXTEND(array_lfa_extend, array_lfa, link)
ARRAY_A_EXTEND(array_iaa_extend, array_iaa, copy)
ARRAY_A_EXTEND(array_faa_extend, array_faa, copy)


#define ARRAY_A_COPY(FUNC_NAME, ARRAY_TYPE, TRANSFER)           \
    void                                                        \
    FUNC_NAME(const ARRAY_TYPE *array, ARRAY_TYPE *copy)        \
    {                                                           \
        unsigned i;                                             \
                                                                \
        if (array != copy) {                                    \
            copy->reset(copy);                                  \
            for (i = 0; i < array->len; i++)                    \
                array->_[i]->TRANSFER(array->_[i], copy->append(copy)); \
        }                                                               \
    }
ARRAY_A_COPY(array_ia_copy, array_ia, copy)
ARRAY_A_COPY(array_fa_copy, array_fa, copy)
ARRAY_A_COPY(array_lia_copy, array_lia, link)
ARRAY_A_COPY(array_lfa_copy, array_lfa, link)
ARRAY_A_COPY(array_iaa_copy, array_iaa, copy)
ARRAY_A_COPY(array_faa_copy, array_faa, copy)


#define ARRAY_A_EQUALS(FUNC_NAME, ARRAY_TYPE)                       \
    int                                                             \
    FUNC_NAME(const ARRAY_TYPE *array, const ARRAY_TYPE *compare)   \
    {                                                               \
        unsigned i;                                                 \
                                                                    \
        if (array->len == compare->len) {                           \
            for (i = 0; i < array->len; i++)                        \
                if (!array->_[i]->equals(array->_[i],               \
                                         compare->_[i]))            \
                    return 0;                                       \
                                                                    \
            return 1;                                               \
        } else                                                      \
            return 0;                                               \
    }
ARRAY_A_EQUALS(array_ia_equals, array_ia)
ARRAY_A_EQUALS(array_fa_equals, array_fa)
ARRAY_A_EQUALS(array_lia_equals, array_lia)
ARRAY_A_EQUALS(array_lfa_equals, array_lfa)
ARRAY_A_EQUALS(array_iaa_equals, array_iaa)
ARRAY_A_EQUALS(array_faa_equals, array_faa)

#define ARRAY_A_PRINT(FUNC_NAME, ARRAY_TYPE)                \
    void                                                    \
    FUNC_NAME(const ARRAY_TYPE *array, FILE* output)        \
    {                                                       \
        unsigned i;                                         \
                                                            \
        putc('[', output);                                  \
        if (array->len == 1) {                              \
            array->_[0]->print(array->_[0], output);        \
        } else if (array->len > 1) {                        \
            for (i = 0; i < array->len - 1; i++) {          \
                array->_[i]->print(array->_[i], output);    \
                fprintf(output, ", ");                      \
            }                                               \
            array->_[i]->print(array->_[i], output);        \
        }                                                   \
        putc(']', output);                                  \
    }
ARRAY_A_PRINT(array_ia_print, array_ia)
ARRAY_A_PRINT(array_fa_print, array_fa)
ARRAY_A_PRINT(array_lia_print, array_lia)
ARRAY_A_PRINT(array_lfa_print, array_lfa)
ARRAY_A_PRINT(array_iaa_print, array_iaa)
ARRAY_A_PRINT(array_faa_print, array_faa)

#define ARRAY_A_SPLIT(FUNC_NAME, ARRAY_TYPE, NEW_FUNC, TRANSFER)        \
    void                                                                \
    FUNC_NAME(const ARRAY_TYPE *array, unsigned count,                  \
              ARRAY_TYPE *head, ARRAY_TYPE *tail)                       \
    {                                                                   \
        /*ensure we don't try to move too many items*/                  \
        unsigned to_head = MIN(count, array->len);                      \
        ARRAY_TYPE *temp;                                               \
        unsigned i;                                                     \
                                                                        \
        if ((head == array) && (tail == array)) {                       \
            /*do nothing*/                                              \
            return;                                                     \
        } else if ((head != array) && (tail == array)) {                \
            /*move "count" values to head and shift the rest down*/     \
                                                                        \
            head->reset(head);                                          \
            for (i = 0; i < to_head; i++)                               \
                array->_[i]->swap(array->_[i], head->append(head));     \
                                                                        \
            temp = NEW_FUNC();                                          \
            for (; i < array->len; i++)                                 \
                array->_[i]->swap(array->_[i], temp->append(temp));     \
                                                                        \
            temp->swap(temp, tail);                                     \
            temp->del(temp);                                            \
        } else if ((head == array) && (tail != array)) {                \
            /*move "count" values from our end to tail and reduce our size*/ \
                                                                        \
            tail->reset(tail);                                          \
            for (i = to_head; i < array->len; i++) {                    \
                array->_[i]->swap(array->_[i], tail->append(tail));     \
                array->_[i]->reset(array->_[i]);                        \
            }                                                           \
            head->len = to_head;                                        \
        } else {                                                        \
            /*copy "count" values to "head" and the remainder to "tail"*/ \
                                                                        \
            head->reset(head);                                          \
            tail->reset(tail);                                          \
            for (i = 0; i < to_head; i++)                               \
                array->_[i]->TRANSFER(array->_[i], head->append(head)); \
                                                                        \
            for (; i < array->len; i++)                                 \
                array->_[i]->TRANSFER(array->_[i], tail->append(tail)); \
        }                                                               \
    }
ARRAY_A_SPLIT(array_ia_split, array_ia, array_ia_new, copy)
ARRAY_A_SPLIT(array_fa_split, array_fa, array_fa_new, copy)
ARRAY_A_SPLIT(array_lia_split, array_lia, array_lia_new, link)
ARRAY_A_SPLIT(array_lfa_split, array_lfa, array_lfa_new, link)
ARRAY_A_SPLIT(array_iaa_split, array_iaa, array_iaa_new, copy)
ARRAY_A_SPLIT(array_faa_split, array_faa, array_faa_new, copy)


#define ARRAY_A_CROSS_SPLIT(FUNC_NAME, ARRAY_TYPE)      \
    void                                                \
    FUNC_NAME(const ARRAY_TYPE *array, unsigned count,  \
              ARRAY_TYPE *head, ARRAY_TYPE *tail)       \
    {                                                   \
        unsigned i;                                     \
                                                        \
        if ((head == array) && (tail == array)) {           \
            /*do nothing*/                                  \
        } else if (head == tail) {                          \
            array->copy(array, head);                       \
        } else if ((head != array) && (tail == array)) {    \
            head->reset(head);                              \
            for (i = 0; i < array->len; i++) {              \
                array->_[i]->split(array->_[i],             \
                                   count,                   \
                                   head->append(head),      \
                                   tail->_[i]);             \
            }                                               \
        } else if ((head == array) && (tail != array)) {    \
            tail->reset(tail);                              \
            for (i = 0; i < array->len; i++) {                  \
                array->_[i]->split(array->_[i],                 \
                                   count,                       \
                                   head->_[i],                  \
                                   tail->append(tail));             \
            }                                                       \
        } else {                                                    \
            head->reset(head);                                      \
            tail->reset(tail);                                      \
            for (i = 0; i < array->len; i++) {                      \
                array->_[i]->split(array->_[i],                     \
                                   count,                           \
                                   head->append(head),              \
                                   tail->append(tail));             \
            }                                                       \
        }                                                           \
    }
ARRAY_A_CROSS_SPLIT(array_ia_cross_split, array_ia)
ARRAY_A_CROSS_SPLIT(array_fa_cross_split, array_fa)
ARRAY_A_CROSS_SPLIT(array_lia_cross_split, array_lia)
ARRAY_A_CROSS_SPLIT(array_lfa_cross_split, array_lfa)


#define ARRAY_A_ZIP(FUNC_NAME, ARRAY_TYPE, ARRAY_DATA_TYPE, NEW_FUNC)   \
    void                                                                \
    FUNC_NAME(const ARRAY_TYPE *array, ARRAY_TYPE *zipped)              \
    {                                                                   \
        if (array != zipped) {                                          \
            unsigned i;                                                 \
            unsigned j;                                                 \
            ARRAY_DATA_TYPE* zipped_row;                                \
            unsigned min_row_length = UINT_MAX;                         \
                                                                        \
            zipped->reset(zipped);                                      \
            if (array->len > 0) {                                       \
                /*find the minimum row length of array's rows*/         \
                for (i = 0; i < array->len; i++)                        \
                    min_row_length = MIN(min_row_length, array->_[i]->len); \
                                                                        \
                /*add a row to "zipped" for each item in array's first row*/ \
                for (i = 0; i < min_row_length; i++) {                  \
                    zipped_row = zipped->append(zipped);                \
                    zipped_row->append(zipped_row, array->_[0]->_[i]);  \
                }                                                       \
                                                                        \
                /*then append new items for each subsequent row*/       \
                for (j = 1; j < array->len; j++) {                      \
                    for (i = 0; i < min_row_length; i++) {              \
                        zipped_row = zipped->_[i];                      \
                        zipped_row->append(zipped_row, array->_[j]->_[i]); \
                    }                                                   \
                }                                                       \
            }                                                           \
        } else {                                                        \
            ARRAY_TYPE *temp = NEW_FUNC();                              \
            FUNC_NAME(array, temp);                                     \
            temp->swap(temp, zipped);                                   \
            temp->del(temp);                                            \
        }                                                               \
    }
ARRAY_A_ZIP(array_ia_zip, array_ia, array_i, array_ia_new)
ARRAY_A_ZIP(array_fa_zip, array_fa, array_f, array_fa_new)


array_fa*
array_fa_new(void)
{
    array_fa* a = malloc(sizeof(array_f));
    unsigned i;

    a->_ = malloc(sizeof(array_f*) * 1);
    a->len = 0;
    a->total_size = 1;

    for (i = 0; i < 1; i++) {
        a->_[i] = array_f_new();
    }

    a->del = array_fa_del;
    a->resize = array_fa_resize;
    a->reset = array_fa_reset;
    a->append = array_fa_append;
    a->extend = array_fa_extend;
    a->equals = array_fa_equals;
    a->copy = array_fa_copy;
    a->swap = array_fa_swap;
    a->split = array_fa_split;
    a->cross_split = array_fa_cross_split;
    a->zip = array_fa_zip;
    a->reverse = array_fa_reverse;
    a->print = array_fa_print;

    return a;
}

struct array_iaa_s*
array_iaa_new(void)
{
    struct array_iaa_s* a = malloc(sizeof(struct array_iaa_s));
    unsigned i;

    a->_ = malloc(sizeof(struct array_ia_s*) * 1);
    a->len = 0;
    a->total_size = 1;

    for (i = 0; i < 1; i++) {
        a->_[i] = array_ia_new();
    }

    a->del = array_iaa_del;
    a->resize = array_iaa_resize;
    a->reset = array_iaa_reset;
    a->append = array_iaa_append;
    a->extend = array_iaa_extend;
    a->equals = array_iaa_equals;
    a->copy = array_iaa_copy;
    a->swap = array_iaa_swap;
    a->split = array_iaa_split;
    a->reverse = array_iaa_reverse;
    a->print = array_iaa_print;

    return a;
}

struct array_faa_s*
array_faa_new(void)
{
    struct array_faa_s* a = malloc(sizeof(struct array_faa_s));
    unsigned i;

    a->_ = malloc(sizeof(struct array_fa_s*) * 1);
    a->len = 0;
    a->total_size = 1;

    for (i = 0; i < 1; i++) {
        a->_[i] = array_fa_new();
    }

    a->del = array_faa_del;
    a->resize = array_faa_resize;
    a->reset = array_faa_reset;
    a->append = array_faa_append;
    a->extend = array_faa_extend;
    a->equals = array_faa_equals;
    a->copy = array_faa_copy;
    a->swap = array_faa_swap;
    a->split = array_faa_split;
    a->reverse = array_faa_reverse;
    a->print = array_faa_print;

    return a;
}

void*
array_o_dummy_copy(void* obj)
{
    return obj; /*does nothing*/
}

void
array_o_dummy_free(void* obj)
{
    return;     /*does nothing*/
}

void
array_o_dummy_print(void* obj, FILE* output)
{
    fprintf(output, "<OBJECT>");
}

struct array_o_s*
array_o_new(void* (*copy)(void* obj),
            void (*free)(void* obj),
            void (*print)(void* obj, FILE* output))
{
    struct array_o_s* a = malloc(sizeof(struct array_o_s));
    a->len = 0;
    a->total_size = 1;
    a->_ = malloc(sizeof(void*) * a->total_size);

    if (copy != NULL)
        a->copy_obj = copy;
    else
        a->copy_obj = array_o_dummy_copy;

    if (free != NULL)
        a->free_obj = free;
    else
        a->free_obj = array_o_dummy_free;

    if (print != NULL)
        a->print_obj = print;
    else
        a->print_obj = array_o_dummy_print;

    a->del = array_o_del;
    a->resize = array_o_resize;
    a->resize_for = array_o_resize_for;
    a->reset = array_o_reset;
    a->reset_for = array_o_reset_for;
    a->append = array_o_append;
    a->vappend = array_o_vappend;
    a->mappend = array_o_mappend;
    a->set = array_o_set;
    a->vset = array_o_vset;
    a->mset = array_o_mset;
    a->extend = array_o_extend;
    a->copy = array_o_copy;
    a->swap = array_o_swap;
    a->head = array_o_head;
    a->tail = array_o_tail;
    a->de_head = array_o_de_head;
    a->de_tail = array_o_de_tail;
    a->split = array_o_split;
    a->print = array_o_print;

    return a;
}

void
array_o_del(struct array_o_s *array)
{
    while (array->len) {
        array->free_obj(array->_[--array->len]);
    }
    free(array->_);
    free(array);
}

void
array_o_reset(struct array_o_s *array)
{
    while (array->len) {
        array->free_obj(array->_[--array->len]);
    }
}

void
array_o_append(struct array_o_s *array, void* value)
{
    if (array->len == array->total_size)
        array->resize(array, array->total_size * 2);

    array->_[array->len++] = array->copy_obj(value);
}

void array_o_vappend(struct array_o_s *array, unsigned count, ...)
{
    void* i;
    va_list ap;

    array->resize(array, array->len + count);
    va_start(ap, count);
    for (; count > 0; count--) {
        i = va_arg(ap, void*);
        array->_[array->len++] = array->copy_obj(i);
    }
    va_end(ap);
}

void
array_o_mappend(struct array_o_s *array, unsigned count, void* value)
{
    array->resize(array, array->len + count);
    for (; count > 0; count--) {
        array->_[array->len++] = array->copy_obj(value);
    }
}

void
array_o_set(struct array_o_s *array, unsigned index, void* value)
{
    assert(index < array->len);
    array->free_obj(array->_[index]);
    array->_[index] = array->copy_obj(value);
}

void array_o_vset(struct array_o_s *array, unsigned count, ...)
{
    void* i;
    va_list ap;

    array->reset_for(array, count);
    va_start(ap, count);
    for (; count > 0; count--) {
        i = va_arg(ap, void*);
        array->_[array->len++] = array->copy_obj(i);
    }
    va_end(ap);
}

void
array_o_mset(struct array_o_s *array, unsigned count, void* value)
{
    array->reset_for(array, count);
    for (; count > 0; count--) {
        array->_[array->len++] = array->copy_obj(value);
    }
}

void
array_o_extend(struct array_o_s *array, const struct array_o_s *to_add)
{
    unsigned i;
    const unsigned len = to_add->len;

    array->resize_for(array, to_add->len);

    for (i = 0; i < len; i++) {
        array->_[array->len++] = array->copy_obj(to_add->_[i]);
    }
}

void
array_o_copy(const struct array_o_s *array, struct array_o_s *copy)
{
    if (array != copy) {
        unsigned i;

        copy->reset_for(copy, array->len);
        for (i = 0; i < array->len; i++) {
            copy->_[copy->len++] = array->copy_obj(array->_[i]);
        }
    }
}

void
array_o_head(const struct array_o_s *array, unsigned count,
             struct array_o_s *head)
{
    const unsigned to_copy = MIN(count, array->len);

    if (head != array) {
        unsigned i;
        head->reset_for(head, to_copy);
        for (i = 0; i < to_copy; i++) {
            head->_[head->len++] = array->copy_obj(array->_[i]);
        }
    } else {
        while (head->len > to_copy) {
            array->free_obj(head->_[--head->len]);
        }
    }
}

void
array_o_tail(const struct array_o_s *array, unsigned count,
             struct array_o_s *tail)
{
    const unsigned to_copy = MIN(count, array->len);

    if (tail != array) {
        unsigned i;

        tail->reset_for(tail, to_copy);
        for (i = array->len - to_copy; i < array->len; i++) {
            tail->_[tail->len++] = array->copy_obj(array->_[i]);
        }
    } else {
        struct array_o_s* temp = array_o_new(array->copy_obj,
                                             array->free_obj,
                                             array->print_obj);
        unsigned i;
        temp->resize(temp, to_copy);
        for (i = array->len - to_copy; i < array->len; i++) {
            temp->_[temp->len++] = array->copy_obj(array->_[i]);
        }
        temp->swap(temp, tail);
        temp->del(temp);
    }
}

void
array_o_de_head(const struct array_o_s *array, unsigned count,
                struct array_o_s *tail)
{
    array->tail(array, array->len - MIN(count, array->len), tail);
}

void
array_o_de_tail(const struct array_o_s *array, unsigned count,
                struct array_o_s *head)
{
    array->head(array, array->len - MIN(count, array->len), head);
}

void
array_o_split(const struct array_o_s *array, unsigned count,
              struct array_o_s *head, struct array_o_s *tail)
{
    const unsigned to_head = MIN(count, array->len);
    const unsigned to_tail = array->len - to_head;

    if ((head == array) && (tail == array)) {
        /*do nothing*/
        return;
    } else if (head == tail) {
        /*copy all data to head*/
        array->copy(array, head);
    } else if ((head == array) && (tail != array)) {
        array->tail(array, to_tail, tail);
        array->head(array, to_head, head);
    } else {
        array->head(array, to_head, head);
        array->tail(array, to_tail, tail);
    }
}

void
array_o_print(const struct array_o_s *array, FILE* output)
{
    unsigned i;
    putc('[', output);
    if (array->len == 1) {
        array->print_obj(array->_[0], output);
    } else if (array->len > 1) {
        for (i = 0; i < array->len - 1; i++) {
            array->print_obj(array->_[i], output);
            fprintf(output, ", ");
        }
        array->print_obj(array->_[i], output);
    }
    putc(']', output);
}
