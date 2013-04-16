#include "xevxml.hpp"
#include <xercesc/util/PlatformUtils.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>

namespace xe=xercesc;
namespace xa=xalanc;

void XmlInitialize(void)
{
  try {
    xe::XMLPlatformUtils::Initialize();
    xa::XalanTransformer::initialize();
  }
  catch (const xe::XMLException& toCatch) {
    // Do your failure processing here
    ABORT();
  }
  return;
}

void XmlFinalize(void)
{
  xa::XalanTransformer::terminate();
  xe::XMLPlatformUtils::Terminate();
}
