//
// Generated file, do not edit! Created by opp_msgc 4.1 from applications/udpapp/UDPAppNCMsg.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "UDPAppNCMsg_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




Register_Class(UDPAppNCMsg);

UDPAppNCMsg::UDPAppNCMsg(const char *name, int kind) : cPacket(name,kind)
{
    this->isRequest_var = true;
    coding_vector_arraysize = 0;
    this->coding_vector_var = 0;
    payload_vector_arraysize = 0;
    this->payload_vector_var = 0;
    this->generation_var = 0;
    this->total_pkt_num_var = 0;
}

UDPAppNCMsg::UDPAppNCMsg(const UDPAppNCMsg& other) : cPacket()
{
    setName(other.getName());
    coding_vector_arraysize = 0;
    this->coding_vector_var = 0;
    payload_vector_arraysize = 0;
    this->payload_vector_var = 0;
    operator=(other);
}

UDPAppNCMsg::~UDPAppNCMsg()
{
    delete [] coding_vector_var;
    delete [] payload_vector_var;
}

UDPAppNCMsg& UDPAppNCMsg::operator=(const UDPAppNCMsg& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->isRequest_var = other.isRequest_var;
    delete [] this->coding_vector_var;
    this->coding_vector_var = (other.coding_vector_arraysize==0) ? NULL : new int[other.coding_vector_arraysize];
    coding_vector_arraysize = other.coding_vector_arraysize;
    for (unsigned int i=0; i<coding_vector_arraysize; i++)
        this->coding_vector_var[i] = other.coding_vector_var[i];
    delete [] this->payload_vector_var;
    this->payload_vector_var = (other.payload_vector_arraysize==0) ? NULL : new int[other.payload_vector_arraysize];
    payload_vector_arraysize = other.payload_vector_arraysize;
    for (unsigned int i=0; i<payload_vector_arraysize; i++)
        this->payload_vector_var[i] = other.payload_vector_var[i];
    this->generation_var = other.generation_var;
    this->total_pkt_num_var = other.total_pkt_num_var;
    return *this;
}

void UDPAppNCMsg::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->isRequest_var);
    b->pack(coding_vector_arraysize);
    doPacking(b,this->coding_vector_var,coding_vector_arraysize);
    b->pack(payload_vector_arraysize);
    doPacking(b,this->payload_vector_var,payload_vector_arraysize);
    doPacking(b,this->generation_var);
    doPacking(b,this->total_pkt_num_var);
}

void UDPAppNCMsg::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->isRequest_var);
    delete [] this->coding_vector_var;
    b->unpack(coding_vector_arraysize);
    if (coding_vector_arraysize==0) {
        this->coding_vector_var = 0;
    } else {
        this->coding_vector_var = new int[coding_vector_arraysize];
        doUnpacking(b,this->coding_vector_var,coding_vector_arraysize);
    }
    delete [] this->payload_vector_var;
    b->unpack(payload_vector_arraysize);
    if (payload_vector_arraysize==0) {
        this->payload_vector_var = 0;
    } else {
        this->payload_vector_var = new int[payload_vector_arraysize];
        doUnpacking(b,this->payload_vector_var,payload_vector_arraysize);
    }
    doUnpacking(b,this->generation_var);
    doUnpacking(b,this->total_pkt_num_var);
}

bool UDPAppNCMsg::getIsRequest() const
{
    return isRequest_var;
}

void UDPAppNCMsg::setIsRequest(bool isRequest_var)
{
    this->isRequest_var = isRequest_var;
}

void UDPAppNCMsg::setCoding_vectorArraySize(unsigned int size)
{
    int *coding_vector_var2 = (size==0) ? NULL : new int[size];
    unsigned int sz = coding_vector_arraysize < size ? coding_vector_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        coding_vector_var2[i] = this->coding_vector_var[i];
    for (unsigned int i=sz; i<size; i++)
        coding_vector_var2[i] = 0;
    coding_vector_arraysize = size;
    delete [] this->coding_vector_var;
    this->coding_vector_var = coding_vector_var2;
}

unsigned int UDPAppNCMsg::getCoding_vectorArraySize() const
{
    return coding_vector_arraysize;
}

int UDPAppNCMsg::getCoding_vector(unsigned int k) const
{
    if (k>=coding_vector_arraysize) throw cRuntimeError("Array of size %d indexed by %d", coding_vector_arraysize, k);
    return coding_vector_var[k];
}

void UDPAppNCMsg::setCoding_vector(unsigned int k, int coding_vector_var)
{
    if (k>=coding_vector_arraysize) throw cRuntimeError("Array of size %d indexed by %d", coding_vector_arraysize, k);
    this->coding_vector_var[k]=coding_vector_var;
}

void UDPAppNCMsg::setPayload_vectorArraySize(unsigned int size)
{
    int *payload_vector_var2 = (size==0) ? NULL : new int[size];
    unsigned int sz = payload_vector_arraysize < size ? payload_vector_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        payload_vector_var2[i] = this->payload_vector_var[i];
    for (unsigned int i=sz; i<size; i++)
        payload_vector_var2[i] = 0;
    payload_vector_arraysize = size;
    delete [] this->payload_vector_var;
    this->payload_vector_var = payload_vector_var2;
}

unsigned int UDPAppNCMsg::getPayload_vectorArraySize() const
{
    return payload_vector_arraysize;
}

int UDPAppNCMsg::getPayload_vector(unsigned int k) const
{
    if (k>=payload_vector_arraysize) throw cRuntimeError("Array of size %d indexed by %d", payload_vector_arraysize, k);
    return payload_vector_var[k];
}

void UDPAppNCMsg::setPayload_vector(unsigned int k, int payload_vector_var)
{
    if (k>=payload_vector_arraysize) throw cRuntimeError("Array of size %d indexed by %d", payload_vector_arraysize, k);
    this->payload_vector_var[k]=payload_vector_var;
}

unsigned int UDPAppNCMsg::getGeneration() const
{
    return generation_var;
}

void UDPAppNCMsg::setGeneration(unsigned int generation_var)
{
    this->generation_var = generation_var;
}

unsigned int UDPAppNCMsg::getTotal_pkt_num() const
{
    return total_pkt_num_var;
}

void UDPAppNCMsg::setTotal_pkt_num(unsigned int total_pkt_num_var)
{
    this->total_pkt_num_var = total_pkt_num_var;
}

class UDPAppNCMsgDescriptor : public cClassDescriptor
{
  public:
    UDPAppNCMsgDescriptor();
    virtual ~UDPAppNCMsgDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(UDPAppNCMsgDescriptor);

UDPAppNCMsgDescriptor::UDPAppNCMsgDescriptor() : cClassDescriptor("UDPAppNCMsg", "cPacket")
{
}

UDPAppNCMsgDescriptor::~UDPAppNCMsgDescriptor()
{
}

bool UDPAppNCMsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<UDPAppNCMsg *>(obj)!=NULL;
}

const char *UDPAppNCMsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int UDPAppNCMsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 5+basedesc->getFieldCount(object) : 5;
}

unsigned int UDPAppNCMsgDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<5) ? fieldTypeFlags[field] : 0;
}

const char *UDPAppNCMsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "isRequest",
        "coding_vector",
        "payload_vector",
        "generation",
        "total_pkt_num",
    };
    return (field>=0 && field<5) ? fieldNames[field] : NULL;
}

int UDPAppNCMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='i' && strcmp(fieldName, "isRequest")==0) return base+0;
    if (fieldName[0]=='c' && strcmp(fieldName, "coding_vector")==0) return base+1;
    if (fieldName[0]=='p' && strcmp(fieldName, "payload_vector")==0) return base+2;
    if (fieldName[0]=='g' && strcmp(fieldName, "generation")==0) return base+3;
    if (fieldName[0]=='t' && strcmp(fieldName, "total_pkt_num")==0) return base+4;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *UDPAppNCMsgDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "bool",
        "int",
        "int",
        "unsigned int",
        "unsigned int",
    };
    return (field>=0 && field<5) ? fieldTypeStrings[field] : NULL;
}

const char *UDPAppNCMsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int UDPAppNCMsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    UDPAppNCMsg *pp = (UDPAppNCMsg *)object; (void)pp;
    switch (field) {
        case 1: return pp->getCoding_vectorArraySize();
        case 2: return pp->getPayload_vectorArraySize();
        default: return 0;
    }
}

std::string UDPAppNCMsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    UDPAppNCMsg *pp = (UDPAppNCMsg *)object; (void)pp;
    switch (field) {
        case 0: return bool2string(pp->getIsRequest());
        case 1: return long2string(pp->getCoding_vector(i));
        case 2: return long2string(pp->getPayload_vector(i));
        case 3: return ulong2string(pp->getGeneration());
        case 4: return ulong2string(pp->getTotal_pkt_num());
        default: return "";
    }
}

bool UDPAppNCMsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    UDPAppNCMsg *pp = (UDPAppNCMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setIsRequest(string2bool(value)); return true;
        case 1: pp->setCoding_vector(i,string2long(value)); return true;
        case 2: pp->setPayload_vector(i,string2long(value)); return true;
        case 3: pp->setGeneration(string2ulong(value)); return true;
        case 4: pp->setTotal_pkt_num(string2ulong(value)); return true;
        default: return false;
    }
}

const char *UDPAppNCMsgDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<5) ? fieldStructNames[field] : NULL;
}

void *UDPAppNCMsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    UDPAppNCMsg *pp = (UDPAppNCMsg *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(UDPAppNCMsg_Butterfly);

UDPAppNCMsg_Butterfly::UDPAppNCMsg_Butterfly(const char *name, int kind) : cPacket(name,kind)
{
    this->isCombination_var = false;
    payload_arraysize = 0;
    this->payload_var = 0;
    this->generation_var = 0;
    this->send_time_var = 0;
}

UDPAppNCMsg_Butterfly::UDPAppNCMsg_Butterfly(const UDPAppNCMsg_Butterfly& other) : cPacket()
{
    setName(other.getName());
    payload_arraysize = 0;
    this->payload_var = 0;
    operator=(other);
}

UDPAppNCMsg_Butterfly::~UDPAppNCMsg_Butterfly()
{
    delete [] payload_var;
}

UDPAppNCMsg_Butterfly& UDPAppNCMsg_Butterfly::operator=(const UDPAppNCMsg_Butterfly& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->isCombination_var = other.isCombination_var;
    delete [] this->payload_var;
    this->payload_var = (other.payload_arraysize==0) ? NULL : new unsigned char[other.payload_arraysize];
    payload_arraysize = other.payload_arraysize;
    for (unsigned int i=0; i<payload_arraysize; i++)
        this->payload_var[i] = other.payload_var[i];
    this->generation_var = other.generation_var;
    this->send_time_var = other.send_time_var;
    return *this;
}

void UDPAppNCMsg_Butterfly::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->isCombination_var);
    b->pack(payload_arraysize);
    doPacking(b,this->payload_var,payload_arraysize);
    doPacking(b,this->generation_var);
    doPacking(b,this->send_time_var);
}

void UDPAppNCMsg_Butterfly::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->isCombination_var);
    delete [] this->payload_var;
    b->unpack(payload_arraysize);
    if (payload_arraysize==0) {
        this->payload_var = 0;
    } else {
        this->payload_var = new unsigned char[payload_arraysize];
        doUnpacking(b,this->payload_var,payload_arraysize);
    }
    doUnpacking(b,this->generation_var);
    doUnpacking(b,this->send_time_var);
}

bool UDPAppNCMsg_Butterfly::getIsCombination() const
{
    return isCombination_var;
}

void UDPAppNCMsg_Butterfly::setIsCombination(bool isCombination_var)
{
    this->isCombination_var = isCombination_var;
}

void UDPAppNCMsg_Butterfly::setPayloadArraySize(unsigned int size)
{
    unsigned char *payload_var2 = (size==0) ? NULL : new unsigned char[size];
    unsigned int sz = payload_arraysize < size ? payload_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        payload_var2[i] = this->payload_var[i];
    for (unsigned int i=sz; i<size; i++)
        payload_var2[i] = 0;
    payload_arraysize = size;
    delete [] this->payload_var;
    this->payload_var = payload_var2;
}

unsigned int UDPAppNCMsg_Butterfly::getPayloadArraySize() const
{
    return payload_arraysize;
}

unsigned char UDPAppNCMsg_Butterfly::getPayload(unsigned int k) const
{
    if (k>=payload_arraysize) throw cRuntimeError("Array of size %d indexed by %d", payload_arraysize, k);
    return payload_var[k];
}

void UDPAppNCMsg_Butterfly::setPayload(unsigned int k, unsigned char payload_var)
{
    if (k>=payload_arraysize) throw cRuntimeError("Array of size %d indexed by %d", payload_arraysize, k);
    this->payload_var[k]=payload_var;
}

unsigned int UDPAppNCMsg_Butterfly::getGeneration() const
{
    return generation_var;
}

void UDPAppNCMsg_Butterfly::setGeneration(unsigned int generation_var)
{
    this->generation_var = generation_var;
}

double UDPAppNCMsg_Butterfly::getSend_time() const
{
    return send_time_var;
}

void UDPAppNCMsg_Butterfly::setSend_time(double send_time_var)
{
    this->send_time_var = send_time_var;
}

class UDPAppNCMsg_ButterflyDescriptor : public cClassDescriptor
{
  public:
    UDPAppNCMsg_ButterflyDescriptor();
    virtual ~UDPAppNCMsg_ButterflyDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(UDPAppNCMsg_ButterflyDescriptor);

UDPAppNCMsg_ButterflyDescriptor::UDPAppNCMsg_ButterflyDescriptor() : cClassDescriptor("UDPAppNCMsg_Butterfly", "cPacket")
{
}

UDPAppNCMsg_ButterflyDescriptor::~UDPAppNCMsg_ButterflyDescriptor()
{
}

bool UDPAppNCMsg_ButterflyDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<UDPAppNCMsg_Butterfly *>(obj)!=NULL;
}

const char *UDPAppNCMsg_ButterflyDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int UDPAppNCMsg_ButterflyDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int UDPAppNCMsg_ButterflyDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *UDPAppNCMsg_ButterflyDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "isCombination",
        "payload",
        "generation",
        "send_time",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int UDPAppNCMsg_ButterflyDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='i' && strcmp(fieldName, "isCombination")==0) return base+0;
    if (fieldName[0]=='p' && strcmp(fieldName, "payload")==0) return base+1;
    if (fieldName[0]=='g' && strcmp(fieldName, "generation")==0) return base+2;
    if (fieldName[0]=='s' && strcmp(fieldName, "send_time")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *UDPAppNCMsg_ButterflyDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "bool",
        "unsigned char",
        "unsigned int",
        "double",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *UDPAppNCMsg_ButterflyDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int UDPAppNCMsg_ButterflyDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    UDPAppNCMsg_Butterfly *pp = (UDPAppNCMsg_Butterfly *)object; (void)pp;
    switch (field) {
        case 1: return pp->getPayloadArraySize();
        default: return 0;
    }
}

std::string UDPAppNCMsg_ButterflyDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    UDPAppNCMsg_Butterfly *pp = (UDPAppNCMsg_Butterfly *)object; (void)pp;
    switch (field) {
        case 0: return bool2string(pp->getIsCombination());
        case 1: return ulong2string(pp->getPayload(i));
        case 2: return ulong2string(pp->getGeneration());
        case 3: return double2string(pp->getSend_time());
        default: return "";
    }
}

bool UDPAppNCMsg_ButterflyDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    UDPAppNCMsg_Butterfly *pp = (UDPAppNCMsg_Butterfly *)object; (void)pp;
    switch (field) {
        case 0: pp->setIsCombination(string2bool(value)); return true;
        case 1: pp->setPayload(i,string2ulong(value)); return true;
        case 2: pp->setGeneration(string2ulong(value)); return true;
        case 3: pp->setSend_time(string2double(value)); return true;
        default: return false;
    }
}

const char *UDPAppNCMsg_ButterflyDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<4) ? fieldStructNames[field] : NULL;
}

void *UDPAppNCMsg_ButterflyDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    UDPAppNCMsg_Butterfly *pp = (UDPAppNCMsg_Butterfly *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


