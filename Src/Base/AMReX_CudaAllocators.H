#ifndef CUDAALLOCATORS_H_
#define CUDAALLOCATORS_H_

#include <map>
#include <memory>
#include <iostream>
#include <limits>
#include <AMReX_Print.H>
#include <AMReX.H>

#ifdef AMREX_USE_CUDA
#include <cuda.h>
#include <driver_types.h>
#include <cuda_runtime.h>
#endif

namespace amrex {

#ifdef AMREX_USE_CUDA

template<typename T>
class CudaManagedAllocator {
public :

    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    
    template<typename U>
    struct rebind {
        typedef CudaManagedAllocator<U> other;
    };
    
    inline explicit CudaManagedAllocator() {}
    inline         ~CudaManagedAllocator() {}
    inline explicit CudaManagedAllocator(CudaManagedAllocator const&) {}
    template<typename U>
    inline explicit CudaManagedAllocator(CudaManagedAllocator<U> const&) {}
    
    inline pointer       address(reference r)       { return &r; }
    inline const_pointer address(const_reference r) { return &r; }
    
    inline pointer allocate(size_type cnt,
                            typename std::allocator<void>::const_pointer = 0) {
        pointer result = nullptr;
        cudaError_t error = cudaMallocManaged(&result, cnt*sizeof(T));
        if(error != cudaSuccess) {
            amrex::Print() << "Allocation failed in cudaMallocManaged \n";
            amrex::Abort();
        }
        return result;
    }
    
    inline void deallocate(pointer p, size_type) {
        cudaError_t error = cudaFree(p);
        if(error != cudaSuccess) {
            amrex::Print() << "Deallocation failed in cudaFree \n";
            amrex::Abort();
        }
    }
    
    inline size_type max_size() const {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }
    
    inline void construct(pointer p, const T& t) { new(p) T(t); }
    inline void destroy(pointer p) { p->~T(); }
    
    inline bool operator==(CudaManagedAllocator const&)   { return true; }
    inline bool operator!=(CudaManagedAllocator const& a) { return !operator==(a); }
};

template<typename T>
class CudaHostAllocator {
public :

    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    
    template<typename U>
    struct rebind {
        typedef CudaHostAllocator<U> other;
    };
    
    inline explicit CudaHostAllocator() {}
    inline         ~CudaHostAllocator() {}
    inline explicit CudaHostAllocator(CudaHostAllocator const&) {}
    template<typename U>
    inline explicit CudaHostAllocator(CudaHostAllocator<U> const&) {}
    
    inline pointer       address(reference r)       { return &r; }
    inline const_pointer address(const_reference r) { return &r; }
    
    inline pointer allocate(size_type cnt,
                            typename std::allocator<void>::const_pointer = 0) {
        pointer result = nullptr;
        cudaError_t error = cudaMallocHost(&result, cnt*sizeof(T), cudaHostAllocMapped);
        if(error != cudaSuccess) {
            amrex::Print() << "Allocation failed in cudaMallocHost \n";
            amrex::Abort();
        }
        return result;
    }
    
    inline void deallocate(pointer p, size_type) {
        cudaError_t error = cudaFreeHost(p);
        if(error != cudaSuccess) {
            amrex::Print() << "Deallocation failed in cudaFreeHost \n";
            amrex::Abort();
        }
    }
    
    inline size_type max_size() const {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }
    
    inline void construct(pointer p, const T& t) { new(p) T(t); }
    inline void destroy(pointer p) { p->~T(); }
    
    inline bool operator==(CudaHostAllocator const&)   { return true; }
    inline bool operator!=(CudaHostAllocator const& a) { return !operator==(a); }
};

template <typename T>
using CudaVector = std::vector<T, CudaManagedAllocator<T>>;
template <typename K, typename T>
using CudaMap = std::map<K, T, std::less<K>, CudaManagedAllocator<std::pair<K,T>> >;

// When not using CUDA, replace with standard allocator.
// Prevents need to wrap applications in lots of ifdefs.
#else

template <typename T>
using CudaManagedAllocator = std::allocator<T>;
template <typename T>
using CudaHostAllocator = std::allocator<T>;
template <typename T>
using CudaVector = std::vector<T>;
template <typename K, typename T>
using CudaMap = std::map<K,T>;

#endif // CUDA

} // namespace amrex

#endif // CUDAMANAGEDALLOCATOR_H_