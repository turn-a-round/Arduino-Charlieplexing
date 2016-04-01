<#assign licenseFirst = "/*">
<#assign licensePrefix = " * ">
<#assign licenseLast = " */">
<#include "${project.licensePath}">

/* 
 * File:        ${NAME}.${EXTENSION}
 * Description:
 *
 * Author:      ${user}
 * Created on ${DATE}, ${TIME}
 */

#ifndef ${GUARD_NAME}
#define	${GUARD_NAME}

#include<Arduino.h>
namespace ${NAME} {
}
#endif	/* ${GUARD_NAME} */

