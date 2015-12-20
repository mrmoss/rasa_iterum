#ifndef STRING_H
#define STRING_H

#include <Arduino.h>

namespace std
{
    class string
    {
        public:
            string(const char* str=NULL):size_m(0),ptr_m(NULL)
            {
                while(str!=NULL&&str[size_m]!='\0')
                    ++size_m;

                *this=string(str,size_m);
            }

            string(const char* str,const uint32_t size):size_m(size),ptr_m(NULL)
            {
                ptr_m=(char*)malloc(size_m+1);
                ptr_m[size_m]=0;

                if(ptr_m==NULL)
                {
                    size_m=0;
                }
                else
                {
                    for(uint32_t ii=0;ii<size_m;++ii)
                        ptr_m[ii]=str[ii];
                }
            }

            string(const string& copy):size_m(0),ptr_m(NULL)
            {
               *this=string(copy.ptr_m,copy.size_m);
            }

            string& operator=(const string& copy)
            {
                if(this!=&copy)
                {
                    if(ptr_m!=NULL)

                    {
                        free(ptr_m);
                        ptr_m=NULL;
                        size_m=0;
                    }

                    string temp(copy.ptr_m,copy.size_m);
                    swap(temp);
                }

                return *this;
            }

            ~string()
            {
                if(ptr_m!=NULL)
                    free(ptr_m);
            }

            void swap(std::string& str)
            {
                char* temp_ptr=str.ptr_m;
                str.ptr_m=ptr_m;
                ptr_m=temp_ptr;

                uint32_t temp_size=str.size_m;
                str.size_m=size_m;
                size_m=temp_size;
            }

            uint32_t size() const
            {
                return size_m;
            }

            void clear()
            {
                *this=string("",0);
            }

            bool empty() const
            {
                return (size_m>0);
            }

            void resize(const uint32_t size)
            {
                if(size!=size_m)
                {
                    ptr_m=(char*)realloc(ptr_m,size+1);

                    if(ptr_m==NULL)
                    {
                        size_m=0;
                    }
                    else
                    {
                        if(size>=size_m)
                        {
                            for(uint32_t ii=size_m;ii<=size;++ii)
                                ptr_m[ii]='\0';

                            size_m=size;
                        }
                        else
                        {
                            size_m=size;
                            ptr_m[size_m]='\0';
                        }
                    }
                }
            }

            char* c_str() const
            {
                return ptr_m;
            }

            std::string substr(const uint32_t start,const uint32_t size)
            {
                if(start>=size_m)
                    return "";

                size_t clamped_size=size_m-start;

                if(size<clamped_size)
                    clamped_size=size;

                return std::string(ptr_m+start,clamped_size);
            }

            char& operator[](uint32_t ii)
            {
                return ptr_m[ii];
            }

            char& operator[](const uint32_t ii) const
            {
                return ptr_m[ii];
            }

            void print(HardwareSerial& serial) const
            {
                serial.print(ptr_m);
            }

            void println(HardwareSerial& serial) const
            {
                print(serial);
                serial.println("");
            }

        private:
            uint32_t size_m;
            char* ptr_m;
    };

    inline int16_t stoi(const std::string& str)
    {
        if(str.c_str()==NULL)
            return 0;

        return atoi(str.c_str());
    }

    inline int32_t stol(const std::string& str)
    {
        if(str.c_str()==NULL)
            return 0;

        return atol(str.c_str());
    }

    inline uint32_t stoul(const std::string& str)
    {
       return (uint32_t)stol(str);
    }

    inline float stof(const std::string& str)
    {
        if(str.c_str()==NULL)
            return 0;

        return atof(str.c_str());
    }

    inline std::string to_string(const uint32_t value)
    {
        char buffer[100];
        memset(buffer,0,100);
        snprintf(buffer,100,"%lu",value);
        return std::string(buffer);
    }

    inline std::string to_string(const int32_t value)
    {
        char buffer[100];
        memset(buffer,0,100);
        snprintf(buffer,100,"%ld",value);
        return std::string(buffer);
    }

    inline std::string to_string(const float value)
    {
        char buffer[100];
        memset(buffer,0,100);
        snprintf(buffer,100,"%f",value);
        return std::string(buffer);
    }
}

inline std::string operator+(const std::string& lhs,const std::string& rhs)
{
    std::string sum(lhs);
    sum.resize(lhs.size()+rhs.size());

    for(uint32_t ii=0;ii<rhs.size();++ii)
        sum[lhs.size()+ii]=rhs[ii];

    return sum;
}

inline std::string& operator+=(std::string& lhs,const std::string& rhs)
{
    uint32_t size=lhs.size();
    lhs.resize(size+rhs.size());

    for(uint32_t ii=0;ii<rhs.size();++ii)
        lhs[size+ii]=rhs[ii];

    return lhs;
}

inline std::string operator+(const std::string& lhs,const char rhs)
{
    std::string temp=lhs;
    uint32_t size=temp.size();
    temp.resize(size+1);

    if(temp.c_str()!=NULL)
        temp[size]=rhs;

    return temp;
}

inline std::string& operator+=(std::string& lhs,const char rhs)
{
    uint32_t size=lhs.size();
    lhs.resize(size+1);

    if(lhs.c_str()!=NULL)
        lhs[size]=rhs;

    return lhs;
}

#endif
