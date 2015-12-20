#ifndef LIST_H
#define LIST_H

#include <Arduino.h>

template<typename T> class list_node_t
{
    public:
        T data;
        list_node_t* next;
        list_node_t* prev;

        list_node_t():next(NULL),prev(NULL)
        {}

        ~list_node_t()
        {
            if(next!=NULL)
                delete next;
        }

    private:
        list_node_t(const list_node_t& copy);
        list_node_t& operator=(const list_node_t& copy);
};

template<typename T> class list_t
{
    public:
        list_t():size_m(0),head_m(NULL),tail_m(NULL)
        {}

        ~list_t()
        {
            clear();
        }

        inline void clear()
        {
            if(head_m!=NULL)
                delete head_m;
        }

        inline void append(const T& element)
        {
            list_node_t<T>* node=new list_node_t<T>();
            node->data=element;
            node->prev=tail_m;

            if(tail_m!=NULL)
                tail_m->next=node;
 
            if(head_m==NULL)
                head_m=node;

            tail_m=node;
        }

        inline list_node_t<T>* head()
        {
            return head_m;
        }

        inline list_node_t<T>* tail()
        {
            return tail_m;
        }

    private:
        list_t(const list_t& copy);
        list_t& operator=(const list_t& copy);

        uint32_t size_m;
        list_node_t<T>* head_m;
        list_node_t<T>* tail_m;
};

#endif
