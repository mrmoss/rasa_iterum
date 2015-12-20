#ifndef LIST_H
#define LIST_H

#include <Arduino.h>

template<typename T> class list_node_t
{
    public:
        T data;
        list_node_t* next;

        list_node_t():next(NULL)
        {}

        ~list_node_t()
        {
            delete next;
        }

    private:
        list_node_t(const list_node_t& copy);
        list_node_t& operator=(const list_node_t& copy);
};

template<typename T> class list_t
{
    public:
        list_t():size_m(0),head_m(NULL)
        {}

        ~list_t()
        {
            clear();
        }

        void clear()
        {
            if(head_m!=NULL)
                delete head_m;
        }

        void append(const T& element)
        {
            list_node_t<T>* node=new list_node_t<T>();
            node->data=element;
            node->next=head_m;
            head_m=node;
        }

        list_node_t<T>* head()
        {
            return head_m;
        }

    private:
        uint32_t size_m;
        list_node_t<T>* head_m;

        list_t(const list_t& copy);
        list_t& operator=(const list_t& copy);
};

#endif
