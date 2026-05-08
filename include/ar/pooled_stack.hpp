#ifndef AR_POOLED_STACK_H
#define AR_POOLED_STACK_H

#include <boost/context/continuation.hpp>

namespace AsyncRuntime {
    namespace ctx = boost::context;

    template< typename traitsT >
    class pooled_fixedsize_stack {
    private:
        std::size_t     size_;
    public:
        typedef traitsT traits_type;

        pooled_fixedsize_stack(std::size_t size = traits_type::default_size() ) BOOST_NOEXCEPT_OR_NOTHROW
        : size_( size) {}

        ctx::stack_context allocate() {
            char * vp = new char[size_];
            if ( ! vp) {
                throw std::bad_alloc();
            }

            ctx::stack_context sctx;
            sctx.size = size_;
            sctx.sp = static_cast< char * >( vp) + sctx.size;
            return sctx;
        }

        void deallocate( ctx::stack_context & sctx) BOOST_NOEXCEPT_OR_NOTHROW {
            BOOST_ASSERT( sctx.sp);
            char * vp = static_cast< char * >( sctx.sp) - sctx.size;
            delete []vp;
        }
    };
}

#endif //AR_POOLED_STACK_H
