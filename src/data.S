#include "data.h"
	.data
	datafile license             "${PROJECT_SOURCE_DIR}/data/LICENSE"
  datafile version             "${PROJECT_SOURCE_DIR}/data/VERSION"
#ifdef DEVELOPMENT_BUILD
  datafile version_tmpl        "${PROJECT_SOURCE_DIR}/data/VERSION_TEMPLATE_DEV"
#else
  datafile version_tmpl        "${PROJECT_SOURCE_DIR}/data/VERSION_TEMPLATE"
#endif

