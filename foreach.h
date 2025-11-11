#ifndef __FOREACH_H__
#define __FOREACH_H__

template <typename Iterator, typename Func>
void foreach(Iterator begin, Iterator end, Func fn){
    auto iter = begin;
    for(; iter != end ; ++iter )
        fn( *iter );
}

template <typename Container, typename Func>
void foreach(Container &container, Func fn){
    foreach(container.begin(), container.end(), fn);
}

#endif // __FOREACH_H__