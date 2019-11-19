#ifndef REQUEST_H
#define REQUEST_H

#include <cstdint>
#include <iostream>

typedef uint64_t IdType;

// Request information
class SimpleRequest
{
private:
    IdType _id; // request object id
    uint64_t _size; // request size in bytes
    uint64_t _timestamp; //timestamp

public:
    SimpleRequest()
    {
    }
    virtual ~SimpleRequest()
    {
    }

    // Create request
    SimpleRequest(IdType id, uint64_t size)
            : _id(id),
              _size(size)
    {
    }

    SimpleRequest(IdType id, uint64_t size, uint64_t timestamp)
            : _id(id), _size(size), _timestamp(timestamp)
    {
    }


    void reinit(IdType id, uint64_t size)
    {
        _id = id;
        _size = size;
    }

    void reinit(IdType id, uint64_t size, uint64_t timestamp){
        _id = id;
        _size = size;
        _timestamp = timestamp;
    }

    // Print request to stdout
    void print() const
    {
        if(_timestamp == 0)
            std::cout << "id" << getId() << " size " << getSize() << std::endl;
        else
            std::cout << "id" << getId() << " size " << getSize() << " time " << getTimestamp() << std::endl;
    }

    // Get request object id
    IdType getId() const
    {
        return _id;
    }

    // Get request size in bytes
    uint64_t getSize() const
    {
        return _size;
    }

    uint64_t getTimestamp() const {
        return _timestamp;
    }
};


#endif /* REQUEST_H */



