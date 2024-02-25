#include "CDocument.h"
#include "CMDoc.h"
CDocument::CDocument() {}

CDocument::~CDocument() {
    CMDoc::getMDoc().onDocumentDestroyed(this);
}
