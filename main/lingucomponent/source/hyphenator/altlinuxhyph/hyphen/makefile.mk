#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2010 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

PRJ = ..$/..$/..$/..

PRJNAME	= lingucomponent
TARGET	= hyphen_lib
ENABLE_EXCEPTIONS=TRUE
USE_DEFFILE=TRUE

.IF "$(GUI)"=="OS2"
HNJLIB=hyphen.lib
.ELIF "$(GUI)"=="UNX" || "$(COM)"=="GCC"
HNJLIB=-lhyphen
.ELSE
HNJLIB=hyphen.lib
.ENDIF

#----- Settings ---------------------------------------------------------

.INCLUDE : settings.mk

# --- Files --------------------------------------------------------

.IF "$(DISABLE_HYPHEN)" == ""

.IF "$(SYSTEM_HUNSPELL)" != "YES"
HUNSPELL_CFLAGS += -I$(SOLARINCDIR)$/hunspell
.ENDIF

CFLAGS += -I..$/..$/..$/lingutil $(HUNSPELL_CFLAGS)

EXCEPTIONSFILES=	\
		$(SLO)$/hyphenimp.obj

SLOFILES=	\
		$(SLO)$/hreg.obj\
		$(SLO)$/hyphenimp.obj

REALNAME:=hyphen
SHL1TARGET= $(REALNAME)$(DLLPOSTFIX)

SHL1STDLIBS= \
		$(CPPULIB) 	 \
		$(CPPUHELPERLIB) 	 \
        $(I18NISOLANGLIB)   \
		$(TOOLSLIB)		\
		$(SVLLIB)		\
		$(SALLIB)		\
		$(UNOTOOLSLIB)	\
		$(LNGLIB) \
		$(HNJLIB) \
		$(ULINGULIB) \
		$(HUNSPELLLIB)

# build DLL
SHL1LIBS=       $(SLB)$/$(TARGET).lib $(SLB)$/libulingu.lib
SHL1IMPLIB=		i$(REALNAME)
SHL1DEPN=		$(SHL1LIBS)
SHL1DEF=		$(MISC)$/$(SHL1TARGET).def

SHL1VERSIONMAP=$(SOLARENV)/src/component.map

# build DEF file
DEF1NAME	 =$(SHL1TARGET)
DEF1EXPORTFILE=	exports.dxp

# --- Targets ------------------------------------------------------

.INCLUDE : target.mk


ALLTAR : $(MISC)/hyphen.component

$(MISC)/hyphen.component .ERRREMOVE : $(SOLARENV)/bin/createcomponent.xslt \
        hyphen.component
    $(XSLTPROC) --nonet --stringparam uri \
        '$(COMPONENTPREFIX_BASIS_NATIVE)$(SHL1TARGETN:f)' -o $@ \
        $(SOLARENV)/bin/createcomponent.xslt hyphen.component

.ELSE
all:
	@echo "hyphen disabled"

.ENDIF
