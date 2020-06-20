//
// Generated file, do not edit! Created by nedtool 5.5 from veins/modules/application/traci/ASLBSM.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "ASLBSM_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace veins {

// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(ASLBSM)

ASLBSM::ASLBSM(const char *name, short kind) : ::veins::DemoSafetyMessage(name,kind)
{
    this->senderAddress = -1;
    this->vehId = "";
    this->serial = 0;
}

ASLBSM::ASLBSM(const ASLBSM& other) : ::veins::DemoSafetyMessage(other)
{
    copy(other);
}

ASLBSM::~ASLBSM()
{
}

ASLBSM& ASLBSM::operator=(const ASLBSM& other)
{
    if (this==&other) return *this;
    ::veins::DemoSafetyMessage::operator=(other);
    copy(other);
    return *this;
}

void ASLBSM::copy(const ASLBSM& other)
{
    this->demoData = other.demoData;
    this->senderAddress = other.senderAddress;
    this->vehId = other.vehId;
    this->serial = other.serial;
    this->heading = other.heading;
    this->direction = other.direction;
}

void ASLBSM::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::veins::DemoSafetyMessage::parsimPack(b);
    doParsimPacking(b,this->demoData);
    doParsimPacking(b,this->senderAddress);
    doParsimPacking(b,this->vehId);
    doParsimPacking(b,this->serial);
    doParsimPacking(b,this->heading);
    doParsimPacking(b,this->direction);
}

void ASLBSM::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::veins::DemoSafetyMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->demoData);
    doParsimUnpacking(b,this->senderAddress);
    doParsimUnpacking(b,this->vehId);
    doParsimUnpacking(b,this->serial);
    doParsimUnpacking(b,this->heading);
    doParsimUnpacking(b,this->direction);
}

const char * ASLBSM::getDemoData() const
{
    return this->demoData.c_str();
}

void ASLBSM::setDemoData(const char * demoData)
{
    this->demoData = demoData;
}

LAddress::L2Type& ASLBSM::getSenderAddress()
{
    return this->senderAddress;
}

void ASLBSM::setSenderAddress(const LAddress::L2Type& senderAddress)
{
    this->senderAddress = senderAddress;
}

const char * ASLBSM::getVehId() const
{
    return this->vehId.c_str();
}

void ASLBSM::setVehId(const char * vehId)
{
    this->vehId = vehId;
}

int ASLBSM::getSerial() const
{
    return this->serial;
}

void ASLBSM::setSerial(int serial)
{
    this->serial = serial;
}

Heading& ASLBSM::getHeading()
{
    return this->heading;
}

void ASLBSM::setHeading(const Heading& heading)
{
    this->heading = heading;
}

Coord& ASLBSM::getDirection()
{
    return this->direction;
}

void ASLBSM::setDirection(const Coord& direction)
{
    this->direction = direction;
}

class ASLBSMDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    ASLBSMDescriptor();
    virtual ~ASLBSMDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(ASLBSMDescriptor)

ASLBSMDescriptor::ASLBSMDescriptor() : omnetpp::cClassDescriptor("veins::ASLBSM", "veins::DemoSafetyMessage")
{
    propertynames = nullptr;
}

ASLBSMDescriptor::~ASLBSMDescriptor()
{
    delete[] propertynames;
}

bool ASLBSMDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ASLBSM *>(obj)!=nullptr;
}

const char **ASLBSMDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *ASLBSMDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int ASLBSMDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 6+basedesc->getFieldCount() : 6;
}

unsigned int ASLBSMDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<6) ? fieldTypeFlags[field] : 0;
}

const char *ASLBSMDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "demoData",
        "senderAddress",
        "vehId",
        "serial",
        "heading",
        "direction",
    };
    return (field>=0 && field<6) ? fieldNames[field] : nullptr;
}

int ASLBSMDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='d' && strcmp(fieldName, "demoData")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderAddress")==0) return base+1;
    if (fieldName[0]=='v' && strcmp(fieldName, "vehId")==0) return base+2;
    if (fieldName[0]=='s' && strcmp(fieldName, "serial")==0) return base+3;
    if (fieldName[0]=='h' && strcmp(fieldName, "heading")==0) return base+4;
    if (fieldName[0]=='d' && strcmp(fieldName, "direction")==0) return base+5;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *ASLBSMDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",
        "LAddress::L2Type",
        "string",
        "int",
        "Heading",
        "Coord",
    };
    return (field>=0 && field<6) ? fieldTypeStrings[field] : nullptr;
}

const char **ASLBSMDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ASLBSMDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ASLBSMDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    ASLBSM *pp = (ASLBSM *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *ASLBSMDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ASLBSM *pp = (ASLBSM *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ASLBSMDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ASLBSM *pp = (ASLBSM *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getDemoData());
        case 1: {std::stringstream out; out << pp->getSenderAddress(); return out.str();}
        case 2: return oppstring2string(pp->getVehId());
        case 3: return long2string(pp->getSerial());
        case 4: {std::stringstream out; out << pp->getHeading(); return out.str();}
        case 5: {std::stringstream out; out << pp->getDirection(); return out.str();}
        default: return "";
    }
}

bool ASLBSMDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    ASLBSM *pp = (ASLBSM *)object; (void)pp;
    switch (field) {
        case 0: pp->setDemoData((value)); return true;
        case 2: pp->setVehId((value)); return true;
        case 3: pp->setSerial(string2long(value)); return true;
        default: return false;
    }
}

const char *ASLBSMDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 1: return omnetpp::opp_typename(typeid(LAddress::L2Type));
        case 4: return omnetpp::opp_typename(typeid(Heading));
        case 5: return omnetpp::opp_typename(typeid(Coord));
        default: return nullptr;
    };
}

void *ASLBSMDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    ASLBSM *pp = (ASLBSM *)object; (void)pp;
    switch (field) {
        case 1: return (void *)(&pp->getSenderAddress()); break;
        case 4: return (void *)(&pp->getHeading()); break;
        case 5: return (void *)(&pp->getDirection()); break;
        default: return nullptr;
    }
}

} // namespace veins

