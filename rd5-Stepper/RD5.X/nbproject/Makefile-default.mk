#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/RD5.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/RD5.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../../Source/portable/MPLAB/PIC32MX/port.c ../../../Source/portable/MPLAB/PIC32MX/port_asm.S ../../../Source/queue.c ../../../Source/tasks.c ../../../Source/list.c ../../../Source/portable/MemMang/heap_2.c ../main.c ../stepper.c ../FreeRTOS_Common_Tasks.c ../chipKIT_PRO_MX7.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/332309696/port.o ${OBJECTDIR}/_ext/332309696/port_asm.o ${OBJECTDIR}/_ext/449926602/queue.o ${OBJECTDIR}/_ext/449926602/tasks.o ${OBJECTDIR}/_ext/449926602/list.o ${OBJECTDIR}/_ext/1884096877/heap_2.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/stepper.o ${OBJECTDIR}/_ext/1472/FreeRTOS_Common_Tasks.o ${OBJECTDIR}/_ext/1472/chipKIT_PRO_MX7.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/332309696/port.o.d ${OBJECTDIR}/_ext/332309696/port_asm.o.d ${OBJECTDIR}/_ext/449926602/queue.o.d ${OBJECTDIR}/_ext/449926602/tasks.o.d ${OBJECTDIR}/_ext/449926602/list.o.d ${OBJECTDIR}/_ext/1884096877/heap_2.o.d ${OBJECTDIR}/_ext/1472/main.o.d ${OBJECTDIR}/_ext/1472/stepper.o.d ${OBJECTDIR}/_ext/1472/FreeRTOS_Common_Tasks.o.d ${OBJECTDIR}/_ext/1472/chipKIT_PRO_MX7.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/332309696/port.o ${OBJECTDIR}/_ext/332309696/port_asm.o ${OBJECTDIR}/_ext/449926602/queue.o ${OBJECTDIR}/_ext/449926602/tasks.o ${OBJECTDIR}/_ext/449926602/list.o ${OBJECTDIR}/_ext/1884096877/heap_2.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/stepper.o ${OBJECTDIR}/_ext/1472/FreeRTOS_Common_Tasks.o ${OBJECTDIR}/_ext/1472/chipKIT_PRO_MX7.o

# Source Files
SOURCEFILES=../../../Source/portable/MPLAB/PIC32MX/port.c ../../../Source/portable/MPLAB/PIC32MX/port_asm.S ../../../Source/queue.c ../../../Source/tasks.c ../../../Source/list.c ../../../Source/portable/MemMang/heap_2.c ../main.c ../stepper.c ../FreeRTOS_Common_Tasks.c ../chipKIT_PRO_MX7.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/RD5.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX795F512L
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/332309696/port_asm.o: ../../../Source/portable/MPLAB/PIC32MX/port_asm.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/332309696 
	@${RM} ${OBJECTDIR}/_ext/332309696/port_asm.o.d 
	@${RM} ${OBJECTDIR}/_ext/332309696/port_asm.o 
	@${RM} ${OBJECTDIR}/_ext/332309696/port_asm.o.ok ${OBJECTDIR}/_ext/332309696/port_asm.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/332309696/port_asm.o.d" "${OBJECTDIR}/_ext/332309696/port_asm.o.asm.d" -t $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC} $(MP_EXTRA_AS_PRE)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -c -mprocessor=$(MP_PROCESSOR_OPTION) -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -MMD -MF "${OBJECTDIR}/_ext/332309696/port_asm.o.d"  -o ${OBJECTDIR}/_ext/332309696/port_asm.o ../../../Source/portable/MPLAB/PIC32MX/port_asm.S  -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/332309696/port_asm.o.asm.d",--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--gdwarf-2,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1 -I"../../../Source/portable/MPLAB/PIC32MX" -I".."
	
else
${OBJECTDIR}/_ext/332309696/port_asm.o: ../../../Source/portable/MPLAB/PIC32MX/port_asm.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/332309696 
	@${RM} ${OBJECTDIR}/_ext/332309696/port_asm.o.d 
	@${RM} ${OBJECTDIR}/_ext/332309696/port_asm.o 
	@${RM} ${OBJECTDIR}/_ext/332309696/port_asm.o.ok ${OBJECTDIR}/_ext/332309696/port_asm.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/332309696/port_asm.o.d" "${OBJECTDIR}/_ext/332309696/port_asm.o.asm.d" -t $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC} $(MP_EXTRA_AS_PRE)  -c -mprocessor=$(MP_PROCESSOR_OPTION) -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -MMD -MF "${OBJECTDIR}/_ext/332309696/port_asm.o.d"  -o ${OBJECTDIR}/_ext/332309696/port_asm.o ../../../Source/portable/MPLAB/PIC32MX/port_asm.S  -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/332309696/port_asm.o.asm.d",--gdwarf-2 -I"../../../Source/portable/MPLAB/PIC32MX" -I".."
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/332309696/port.o: ../../../Source/portable/MPLAB/PIC32MX/port.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/332309696 
	@${RM} ${OBJECTDIR}/_ext/332309696/port.o.d 
	@${RM} ${OBJECTDIR}/_ext/332309696/port.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/332309696/port.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/332309696/port.o.d" -o ${OBJECTDIR}/_ext/332309696/port.o ../../../Source/portable/MPLAB/PIC32MX/port.c   
	
${OBJECTDIR}/_ext/449926602/queue.o: ../../../Source/queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/449926602 
	@${RM} ${OBJECTDIR}/_ext/449926602/queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/449926602/queue.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/449926602/queue.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/449926602/queue.o.d" -o ${OBJECTDIR}/_ext/449926602/queue.o ../../../Source/queue.c   
	
${OBJECTDIR}/_ext/449926602/tasks.o: ../../../Source/tasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/449926602 
	@${RM} ${OBJECTDIR}/_ext/449926602/tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/449926602/tasks.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/449926602/tasks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/449926602/tasks.o.d" -o ${OBJECTDIR}/_ext/449926602/tasks.o ../../../Source/tasks.c   
	
${OBJECTDIR}/_ext/449926602/list.o: ../../../Source/list.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/449926602 
	@${RM} ${OBJECTDIR}/_ext/449926602/list.o.d 
	@${RM} ${OBJECTDIR}/_ext/449926602/list.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/449926602/list.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/449926602/list.o.d" -o ${OBJECTDIR}/_ext/449926602/list.o ../../../Source/list.c   
	
${OBJECTDIR}/_ext/1884096877/heap_2.o: ../../../Source/portable/MemMang/heap_2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1884096877 
	@${RM} ${OBJECTDIR}/_ext/1884096877/heap_2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1884096877/heap_2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1884096877/heap_2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/1884096877/heap_2.o.d" -o ${OBJECTDIR}/_ext/1884096877/heap_2.o ../../../Source/portable/MemMang/heap_2.c   
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d" -o ${OBJECTDIR}/_ext/1472/main.o ../main.c   
	
${OBJECTDIR}/_ext/1472/stepper.o: ../stepper.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/stepper.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/stepper.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/stepper.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/stepper.o.d" -o ${OBJECTDIR}/_ext/1472/stepper.o ../stepper.c   
	
${OBJECTDIR}/_ext/1472/FreeRTOS_Common_Tasks.o: ../FreeRTOS_Common_Tasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/FreeRTOS_Common_Tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/FreeRTOS_Common_Tasks.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/FreeRTOS_Common_Tasks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/FreeRTOS_Common_Tasks.o.d" -o ${OBJECTDIR}/_ext/1472/FreeRTOS_Common_Tasks.o ../FreeRTOS_Common_Tasks.c   
	
${OBJECTDIR}/_ext/1472/chipKIT_PRO_MX7.o: ../chipKIT_PRO_MX7.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/chipKIT_PRO_MX7.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/chipKIT_PRO_MX7.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/chipKIT_PRO_MX7.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/chipKIT_PRO_MX7.o.d" -o ${OBJECTDIR}/_ext/1472/chipKIT_PRO_MX7.o ../chipKIT_PRO_MX7.c   
	
else
${OBJECTDIR}/_ext/332309696/port.o: ../../../Source/portable/MPLAB/PIC32MX/port.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/332309696 
	@${RM} ${OBJECTDIR}/_ext/332309696/port.o.d 
	@${RM} ${OBJECTDIR}/_ext/332309696/port.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/332309696/port.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/332309696/port.o.d" -o ${OBJECTDIR}/_ext/332309696/port.o ../../../Source/portable/MPLAB/PIC32MX/port.c   
	
${OBJECTDIR}/_ext/449926602/queue.o: ../../../Source/queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/449926602 
	@${RM} ${OBJECTDIR}/_ext/449926602/queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/449926602/queue.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/449926602/queue.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/449926602/queue.o.d" -o ${OBJECTDIR}/_ext/449926602/queue.o ../../../Source/queue.c   
	
${OBJECTDIR}/_ext/449926602/tasks.o: ../../../Source/tasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/449926602 
	@${RM} ${OBJECTDIR}/_ext/449926602/tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/449926602/tasks.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/449926602/tasks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/449926602/tasks.o.d" -o ${OBJECTDIR}/_ext/449926602/tasks.o ../../../Source/tasks.c   
	
${OBJECTDIR}/_ext/449926602/list.o: ../../../Source/list.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/449926602 
	@${RM} ${OBJECTDIR}/_ext/449926602/list.o.d 
	@${RM} ${OBJECTDIR}/_ext/449926602/list.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/449926602/list.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/449926602/list.o.d" -o ${OBJECTDIR}/_ext/449926602/list.o ../../../Source/list.c   
	
${OBJECTDIR}/_ext/1884096877/heap_2.o: ../../../Source/portable/MemMang/heap_2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1884096877 
	@${RM} ${OBJECTDIR}/_ext/1884096877/heap_2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1884096877/heap_2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1884096877/heap_2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/1884096877/heap_2.o.d" -o ${OBJECTDIR}/_ext/1884096877/heap_2.o ../../../Source/portable/MemMang/heap_2.c   
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d" -o ${OBJECTDIR}/_ext/1472/main.o ../main.c   
	
${OBJECTDIR}/_ext/1472/stepper.o: ../stepper.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/stepper.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/stepper.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/stepper.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/stepper.o.d" -o ${OBJECTDIR}/_ext/1472/stepper.o ../stepper.c   
	
${OBJECTDIR}/_ext/1472/FreeRTOS_Common_Tasks.o: ../FreeRTOS_Common_Tasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/FreeRTOS_Common_Tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/FreeRTOS_Common_Tasks.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/FreeRTOS_Common_Tasks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/FreeRTOS_Common_Tasks.o.d" -o ${OBJECTDIR}/_ext/1472/FreeRTOS_Common_Tasks.o ../FreeRTOS_Common_Tasks.c   
	
${OBJECTDIR}/_ext/1472/chipKIT_PRO_MX7.o: ../chipKIT_PRO_MX7.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/chipKIT_PRO_MX7.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/chipKIT_PRO_MX7.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/chipKIT_PRO_MX7.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -fomit-frame-pointer -DMPLAB_PIC32MX_PORT -I".." -I"../../common/include" -I"../../../source/portable/mplab/pic32mx" -I"../../../source/include" -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/chipKIT_PRO_MX7.o.d" -o ${OBJECTDIR}/_ext/1472/chipKIT_PRO_MX7.o ../chipKIT_PRO_MX7.c   
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/RD5.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mdebugger -D__MPLAB_DEBUGGER_PK3=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/RD5.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}           -mreserve=data@0x0:0x1FC -mreserve=boot@0x1FC02000:0x1FC02FEF -mreserve=boot@0x1FC02000:0x1FC024FF  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,--defsym=_min_heap_size=0,-L"..",-Map="${DISTDIR}/RD5.X.${IMAGE_TYPE}.map",-O3
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/RD5.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/RD5.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=0,-L"..",-Map="${DISTDIR}/RD5.X.${IMAGE_TYPE}.map",-O3
	${MP_CC_DIR}\\xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/RD5.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
