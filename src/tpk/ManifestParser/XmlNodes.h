#ifndef __TPK_MANIFESTPARSER_XMLNODES_H__
#define __TPK_MANIFESTPARSER_XMLNODES_H__

#include <vector>

#include "Exception.h"
#include <libxml/globals.h>
#include <libxml/xmlstring.h>
#include <libxml/xmlreader.h>

namespace tpk {
namespace ManifestParser {

class InvalidNodeTypeException: public tpk::Exception {};
class InvalidNodeNameException: public tpk::Exception {};

class XmlNode {
  public:
    xmlChar *_nodeName;
    int depth;
    int nodeType;
    xmlChar *name;    // The name of node
    xmlChar *data;    // Internal text of the node

    XmlNode() : name(NULL), data(NULL), depth(-1), nodeType(XML_READER_TYPE_NONE) {
    };

    virtual ~XmlNode() {
      free();
    };

    template <typename T>
    void setChildNode(const char* nodeName, T &node) {
      // TODO: implement later
    }

    virtual void readBasicValues(xmlTextReaderPtr reader) {
      depth = xmlTextReaderDepth(reader);
      nodeType = xmlTextReaderNodeType(reader);
      data = xmlTextReaderReadString(reader);
    }

    virtual void init(xmlTextReaderPtr reader) {
      name = xmlTextReaderName(reader);
      if(!xmlStrEqual(name, _nodeName)) {
        std::cerr << "Invalid node!  actual name=" << name << ", expected name=" << _nodeName << endl;
        xmlFree(name);
        throw new InvalidNodeNameException();
      }
      readBasicValues(reader);
    };

    virtual void free() {
      xmlFree(name);
      xmlFree(data);
    }

    //virtual void readAttr
    virtual void setExpectedNodeName(const char* nodeName) {
      _nodeName = (xmlChar *)nodeName;
    };
};

class XmlNodeIcon : public XmlNode {
  public:
    XmlNodeIcon() {
      XmlNode::setExpectedNodeName("icon");
    };
    virtual void init(xmlTextReaderPtr reader) {
      XmlNode::init(reader);
    }
};

class XmlNodeLabel : public XmlNode {
  public:
    XmlNodeLabel() {
      XmlNode::setExpectedNodeName("label");
    };
    virtual void init(xmlTextReaderPtr reader) {
      XmlNode::init(reader);
    }
};

class XmlNodeProfile : public XmlNode {
  public:
    XmlNodeProfile() {
      XmlNode::setExpectedNodeName("profile");
    }
    virtual void init(xmlTextReaderPtr reader) {
      XmlNode::init(reader);
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

    XmlNodeUiApplication() : appid(NULL), exec(NULL), type(NULL), multiple(NULL), taskmanage(NULL), nodisplay(NULL) {
      XmlNode::setExpectedNodeName("ui-application");
    };
    void init(xmlTextReaderPtr reader) {
      XmlNode::init(reader);

      appid = xmlTextReaderGetAttribute(reader, (xmlChar *)"appid");
      exec = xmlTextReaderGetAttribute(reader, (xmlChar *)"exec");
      type = xmlTextReaderGetAttribute(reader, (xmlChar *)"type");
      multiple = xmlTextReaderGetAttribute(reader, (xmlChar *)"multiple");
      taskmanage = xmlTextReaderGetAttribute(reader, (xmlChar *)"taskmanage");
      nodisplay = xmlTextReaderGetAttribute(reader, (xmlChar *)"nodisplay");
    };
    virtual ~XmlNodeUiApplication() {
      xmlFree(appid);
      xmlFree(exec);
      xmlFree(type);
      xmlFree(multiple);
      xmlFree(taskmanage);
      xmlFree(nodisplay);
    };
};


class XmlNodePrivilege : public XmlNode {
  public:
    XmlNodePrivilege() {
      XmlNode::setExpectedNodeName("privilege");
    };
    virtual void init(xmlTextReaderPtr reader) {
      XmlNode::init(reader);
    };
};

class XmlNodePrivileges: public XmlNode {
  public:
    vector<XmlNodePrivilege *> v_privilege;

    XmlNodePrivileges() {
      XmlNode::setExpectedNodeName("privileges");
    };
    ~XmlNodePrivileges() {
      // clear v_privilege
      vector<XmlNodePrivilege *>::iterator it;
      for(it = v_privilege.begin(); it != v_privilege.end(); it++) {
        delete *it;
      }
    }
    virtual void init(xmlTextReaderPtr reader) {
      XmlNode::init(reader);
    }
    virtual void addPrivilege(xmlTextReaderPtr reader) {
      if (xmlStrEqual(xmlTextReaderConstName(reader), (xmlChar *)"privilege")) {
        XmlNodePrivilege *p = new XmlNodePrivilege();
        p->init(reader);
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

    XmlNodeManifest() : xmlns(NULL), api_version(NULL), package(NULL), version(NULL) {
      XmlNode::setExpectedNodeName("manifest");
    };
    void init(xmlTextReaderPtr reader) {
      XmlNode::init(reader);

      xmlns = xmlTextReaderGetAttribute(reader, (xmlChar *)"xmlns");
      api_version = xmlTextReaderGetAttribute(reader, (xmlChar *)"api-version");
      package = xmlTextReaderGetAttribute(reader, (xmlChar *)"package");
      version = xmlTextReaderGetAttribute(reader, (xmlChar *)"version");
    };

    ~XmlNodeManifest() {
      xmlFree(xmlns);
      xmlFree(api_version);
      xmlFree(package);
      xmlFree(version);
    }
};

}   // ns: ManifestParser
}   // ns: tpk
#endif  // __TPK_MANIFESTPARSER_XMLNODES_H__
