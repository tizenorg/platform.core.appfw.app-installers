/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#ifndef TPK_XML_NODES_H_
#define TPK_XML_NODES_H_

#include <libxml/globals.h>
#include <libxml/xmlstring.h>
#include <libxml/xmlreader.h>
#include <vector>
#include "tpk/exception.h"
#include "utils/logging.h"

using std::vector;

namespace tpk {

class InvalidNodeTypeException: public tpk::Exception {};
class InvalidNodeNameException: public tpk::Exception {};


class XmlNode {
  SCOPE_LOG_TAG(XmlNode)

 public:
  int depth;
  int node_type;
  xmlChar* name;    // The name of node
  xmlChar* data;    // Internal text of the node

  XmlNode() : name(NULL), data(NULL), depth(-1),
      node_type(XML_READER_TYPE_NONE) {
  }

  virtual ~XmlNode() {
    Free();
  }

  virtual void ReadBasicValues(xmlTextReaderPtr reader) {
    depth = xmlTextReaderDepth(reader);
    node_type = xmlTextReaderNodeType(reader);
    data = xmlTextReaderReadString(reader);
  }

  virtual void Init(xmlTextReaderPtr reader) {
    name = xmlTextReaderName(reader);
    if (!xmlStrEqual(name, node_name_)) {
      LOG(ERROR) << "Invalid node!  actual name=" << name
          << ", expected name=" << node_name_ << std::endl;
      xmlFree(name);
      throw InvalidNodeNameException();
    }
    ReadBasicValues(reader);
  }

  virtual void Free() {
    xmlFree(name);
    xmlFree(data);
  }

  virtual void SetExpectedNodeName(const char* nodeName) {
    node_name_ = const_cast<xmlChar*>(
        reinterpret_cast<const xmlChar*>(nodeName));
  }


 private:
  xmlChar* node_name_;
};


class XmlNodeIcon : public XmlNode {
 public:
  XmlNodeIcon() {
    XmlNode::SetExpectedNodeName("icon");
  }

  virtual void Init(xmlTextReaderPtr reader) {
    XmlNode::Init(reader);
  }
};


class XmlNodeLabel : public XmlNode {
 public:
  XmlNodeLabel() {
    XmlNode::SetExpectedNodeName("label");
  }
  virtual void Init(xmlTextReaderPtr reader) {
    XmlNode::Init(reader);
  }
};

class XmlNodeProfile : public XmlNode {
 public:
  XmlNodeProfile() {
    XmlNode::SetExpectedNodeName("profile");
  }

  virtual void Init(xmlTextReaderPtr reader) {
    XmlNode::Init(reader);
  }
};

class XmlNodeUiApplication : public XmlNode {
 public:
  xmlChar *appid;
  xmlChar *exec;
  xmlChar *type;
  xmlChar *multiple;
  xmlChar *taskmanage;
  xmlChar *nodisplay;

  XmlNodeIcon icon;
  XmlNodeLabel label;

  XmlNodeUiApplication() : appid(NULL), exec(NULL), type(NULL),
      multiple(NULL), taskmanage(NULL), nodisplay(NULL) {
    XmlNode::SetExpectedNodeName("ui-application");
  }

  void Init(xmlTextReaderPtr reader) {
    XmlNode::Init(reader);

    appid = xmlTextReaderGetAttribute(reader,
        const_cast<xmlChar*>(reinterpret_cast<const xmlChar*>("appid")));
    exec = xmlTextReaderGetAttribute(reader,
        const_cast<xmlChar*>(reinterpret_cast<const xmlChar*>("exec")));
    type = xmlTextReaderGetAttribute(reader,
        const_cast<xmlChar*>(reinterpret_cast<const xmlChar*>("type")));
    multiple = xmlTextReaderGetAttribute(reader,
        const_cast<xmlChar*>(reinterpret_cast<const xmlChar*>("multiple")));
    taskmanage = xmlTextReaderGetAttribute(reader,
        const_cast<xmlChar*>(reinterpret_cast<const xmlChar*>("taskmanage")));
    nodisplay = xmlTextReaderGetAttribute(reader,
        const_cast<xmlChar*>(reinterpret_cast<const xmlChar*>("nodisplay")));
  }
  virtual ~XmlNodeUiApplication() {
    xmlFree(appid);
    xmlFree(exec);
    xmlFree(type);
    xmlFree(multiple);
    xmlFree(taskmanage);
    xmlFree(nodisplay);
  }
};


class XmlNodePrivilege : public XmlNode {
 public:
  XmlNodePrivilege() {
    XmlNode::SetExpectedNodeName("privilege");
  }
  virtual void Init(xmlTextReaderPtr reader) {
    XmlNode::Init(reader);
  }
};

class XmlNodePrivileges: public XmlNode {
 public:
  vector<XmlNodePrivilege *> v_privilege;

  XmlNodePrivileges() {
    XmlNode::SetExpectedNodeName("privileges");
  }
  ~XmlNodePrivileges() {
    // clear v_privilege
    vector<XmlNodePrivilege *>::iterator it;
    for (auto& ptr : v_privilege) {
      delete ptr;
    }
  }
  virtual void Init(xmlTextReaderPtr reader) {
    XmlNode::Init(reader);
  }
  virtual void addPrivilege(xmlTextReaderPtr reader) {
    if (xmlStrEqual(xmlTextReaderConstName(reader),
          reinterpret_cast<const xmlChar *>("privilege"))) {
      XmlNodePrivilege *p = new XmlNodePrivilege();
      p->Init(reader);
      v_privilege.push_back(p);
    }
  }
  vector<XmlNodePrivilege *> getPrivilegeVector(void) {
    return v_privilege;
  }
};

class XmlNodeManifest : public XmlNode {
 public:
  xmlChar *xmlns;
  xmlChar *api_version;
  xmlChar *package;
  xmlChar *version;
  XmlNodeProfile profile;
  XmlNodeUiApplication ui_application;
  XmlNodePrivileges privileges;

  XmlNodeManifest() : xmlns(NULL), api_version(NULL), package(NULL),
      version(NULL) {
    XmlNode::SetExpectedNodeName("manifest");
  }

  void Init(xmlTextReaderPtr reader) {
    XmlNode::Init(reader);

    xmlns = xmlTextReaderGetAttribute(reader,
        reinterpret_cast<const xmlChar*>("xmlns"));
    api_version = xmlTextReaderGetAttribute(reader,
        reinterpret_cast<const xmlChar*>("api-version"));
    package = xmlTextReaderGetAttribute(reader,
        reinterpret_cast<const xmlChar*>("package"));
    version = xmlTextReaderGetAttribute(reader,
        reinterpret_cast<const xmlChar*>("version"));
  }

  ~XmlNodeManifest() {
    xmlFree(xmlns);
    xmlFree(api_version);
    xmlFree(package);
    xmlFree(version);
  }
};

}  // namespace tpk
#endif  // TPK_XML_NODES_H_
