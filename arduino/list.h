#ifndef LIST_H
#define LIST_H

#include <Arduino.h>

template<typename T> class list_t
{
    public:
        class node_t
        {
            public:
                T data;
                node_t* next;
        
                node_t():next(NULL)
                {}
        
                ~node_t()
                {
                    delete next;
                }
        
            private:
                node_t(const node_t& copy);
                node_t& operator=(const node_t& copy);
        };

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
            node_t* node=new node_t();
            node->data=element;
            node->next=head_m;
            head_m=node;
        }

        node_t* head()
        {
            return head_m;
        }

    private:
        uint32_t size_m;
        node_t* head_m;

        list_t(const list_t& copy);
        list_t& operator=(const list_t& copy);
};

#endif
