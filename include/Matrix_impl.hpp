#ifndef HH_MATRIX_IMPL_HH
#define HH_MATRIX_IMPL_HH

#include "Matrix.hpp"

using namespace algebra;
/*
//Constructor
template <class T, StorageOrder Order>
Matrix<T, Order>::Matrix():
m_size{0}
{}
*/
template <class T, StorageOrder Order>
Matrix<T, Order>::Matrix(unsigned int i, unsigned int j)
{
    if (Order==StorageOrder::RowWise)
    {
        m_size[0]=i;
        m_size[1]=j;
    }
}
template <class T, StorageOrder Order>
void
Matrix<T, Order>::resize(unsigned int i, unsigned int j){
   if(this->is_compressed())
   {
        uncompress();
   }
    if (Order== StorageOrder::RowWise)
    {
        m_size[0]=i;
        m_size[1]=j;
    }
}
template<class T, StorageOrder Order>
void
Matrix<T, Order>::uncompress(){
    if (is_compressed())
    {
        if (Order==StorageOrder::RowWise){
            unsigned int j=m_inner_index[0];
            for (auto it=m_inner_index.begin(); it!=(m_inner_index.end()-1); ++it){
                for(auto i=*it; i<*(it+1); ++i){
                    m_data.insert({{j,m_outer_index[i]} , m_val[i]});
                }
                ++j;
            }
        }
        
    }
    m_state=false;
    m_val.clear();
    m_outer_index.clear();
    m_inner_index.clear();
}
//Update Properties
template <class T, StorageOrder Order>
void
Matrix<T, Order>::update_properties()
{
    m_nnz=0;
    m_m=0;

    auto it=m_data.rbegin();
    if (Order==StorageOrder::RowWise){
        m_m=it->first[0]+1;
    }else if(Order==StorageOrder::ColWise){
        m_m=it->first[1]+1;
    }
    m_nnz=m_data.size(); //use the size of the map to retrieve the number of elements 

   
}
//Compress the matrix
template <class T, StorageOrder Order>
void 
Matrix<T, Order>::compress(std::vector<T>           &val,
                    std::vector<unsigned int> &outer_index,
                    std::vector<unsigned int> &inner_index)
{
    update_properties();
    val.reserve(m_nnz); // val has the dimension of the number of non zero elements 
    outer_index.reserve(m_nnz); //outer_index has the dimension of the number of NON (???? PER ORA LI PRENDE TUTTI, ANCHE ZERO)zero elements

    inner_index.reserve(m_m+1);// inner_index has the dimension of nrows+1 
    inner_index.emplace_back(0);
    unsigned int temp_idx{0}, key_old{0};
    int i, j;
    if(Order==StorageOrder::RowWise){
        i=1;
        j=0;
    }else if (Order==StorageOrder::ColWise){
        i=0;
        j=1;
    }
    for (auto [key, value] : m_data)
    {
        std::cout<<"key:" <<key[0]<<" "<<key[1]<<std::endl;
        std::cout<<"values:"<<value<<std::endl;
        val.emplace_back(value);

        outer_index.emplace_back(key[i]);
    
        if (key[j]!=key_old)
        {
            inner_index.emplace_back(temp_idx);
            key_old=key[j];
        }

        temp_idx++;
        //std::cout<<*inner_index.rbegin()<<std::endl;
    }
   inner_index.emplace_back(temp_idx);
   
m_data.clear(); // clear the map after the compress to avoid waste of memory
m_state=true;
m_val=val;
m_inner_index=inner_index;
m_outer_index=outer_index;
}

//Overloading streaming operator
template <class T, StorageOrder Order>
std::ostream& operator<<(std::ostream& out, const Matrix<T, Order>& A)
{
    out<<A.m_data->second <<std::endl;
    return out;
}

//Overload operator* for Matrix-vector multiplication
template<class T, StorageOrder Order>
std::vector<T> operator*(const Matrix<T, Order> &A, const std::vector<T> &b){


    //if(!A.is_compressed()){
    //    A.compress(A.m_val, A.m_outer_index, A.m_inner_index);
    //}
    std::vector<T> output;
    if(Order==StorageOrder::RowWise){
    T temp;
    output.reserve(A.m_inner_index.size()-1);
    for(unsigned int i = 0; i < A.m_inner_index.size()-1; ++i){
        temp = 0.0;
        for(unsigned int j = A.m_inner_index[i]; j<A.m_inner_index[i+1]; ++j){
            temp += A.m_val[j] * b[A.m_outer_index[j]];
        }
        output.emplace_back(temp);
    }
    }else if(Order==StorageOrder::ColWise){
    auto max=std::max_element(A.m_outer_index.begin(), A.m_outer_index.end());
    output.resize(*max+1);
    for(unsigned int i = 0; i < A.m_inner_index.size()-1; ++i){
        for(unsigned int j = A.m_inner_index[i]; j<A.m_inner_index[i+1]; ++j){
           output[A.m_outer_index[j]]+= A.m_val[j] * b[i];
        }
    }
    }
    
    return output;
    
}

#endif // HH_MATRIX_IMPL_HH