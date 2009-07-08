include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


roseQtWidgetsHeaderInstallPath = $(includedir)/qtWidgets

ROSE_QT_WIDGETS_ROOT = $(top_srcdir)/src/roseExtensions/qtWidgets
intern_codeEdit = $(ROSE_QT_WIDGETS_ROOT)/RoseCodeEdit/QCodeEdit

ROSE_QT_WIDGETS_INCLUDES =   -I$(ROSE_QT_WIDGETS_ROOT)\
                             -I$(ROSE_QT_WIDGETS_ROOT)/AsmInstructionBar\
                             -I$(ROSE_QT_WIDGETS_ROOT)/AsmView\
                             -I$(ROSE_QT_WIDGETS_ROOT)/AstBrowserWidget\
                             -I$(ROSE_QT_WIDGETS_ROOT)/AstGraphWidget\
                             -I$(ROSE_QT_WIDGETS_ROOT)/AstProcessing\
                             -I$(ROSE_QT_WIDGETS_ROOT)/BeautifiedAst\
                             -I$(ROSE_QT_WIDGETS_ROOT)/FlopCounter\
                             -I$(ROSE_QT_WIDGETS_ROOT)/InstructionCountAnnotator\
                             -I$(ROSE_QT_WIDGETS_ROOT)/KiviatView\
                             -I$(ROSE_QT_WIDGETS_ROOT)/MetricFilter\
                             -I$(ROSE_QT_WIDGETS_ROOT)/MetricsConfig\
                             -I$(ROSE_QT_WIDGETS_ROOT)/MetricsKiviat\
                             -I$(ROSE_QT_WIDGETS_ROOT)/NodeInfoWidget\
                             -I$(ROSE_QT_WIDGETS_ROOT)/ProjectManager\
                             -I$(ROSE_QT_WIDGETS_ROOT)/PropertyTreeWidget\
                             -I$(ROSE_QT_WIDGETS_ROOT)/QtGradientEditor\
                             -I$(ROSE_QT_WIDGETS_ROOT)/RoseCodeEdit\
                             -I$(ROSE_QT_WIDGETS_ROOT)/RoseFileSelector\
                             -I$(ROSE_QT_WIDGETS_ROOT)/ProjectManager\
                             -I$(ROSE_QT_WIDGETS_ROOT)/InstructionCountAnnotator\
                             -I$(ROSE_QT_WIDGETS_ROOT)/TaskSystem\
                             -I$(ROSE_QT_WIDGETS_ROOT)/util\
                             -I$(intern_codeEdit)\
                             -I$(intern_codeEdit)/widgets\
                             -I$(intern_codeEdit)/document\
                             -I$(intern_codeEdit)/qnfa

# The includes for compiling RoseQtWidgets

INCLUDES = -I. \
           -I$(ROSE_QT_WIDGETS_ROOT)\
           $(ROSE_INCLUDES)\
           $(QT_INCLUDES)\
           $(ROSE_QT_WIDGETS_INCLUDES)
              
# ------------- MOC ---------------------

if QROSE_USE_WINDOWS
moc_%.cxx: $(srcdir)/%.h 
	$(MOC) $(INCLUDES) -o  $@ `cygpath -w $<` -p$(srcdir)
else
moc_%.cxx: $(srcdir)/%.h 
	@echo "  Qt-MOC $(notdir $< )" && $(MOC) $(QT_INCLUDES) -o  $@ $<
endif 


# ------------- UIC ---------------------

if QROSE_USE_WINDOWS
uic_%.h: $(srcdir)/%.ui 
	$(UIC) -o $@ `cygpath -w $<` -p$(srcdir)
else
ui_%.h: $(srcdir)/%.ui 
	@echo "  Qt-UIC  $(notdir  $< )" && $(UIC) -o $@ $<
endif 


# ------------- RCC --------------------

if QROSE_USE_WINDOWS
qrc_%.cpp: $(srcdir)/%.qrc 
	$(RCC) -name $(patsubst %.qrc, %, $(notdir  $< ) ) -o $@ `cygpath -w $<` -p$(srcdir)
else
qrc_%.cpp: $(srcdir)/%.qrc
	@echo "  Qt-RCC $(RCC) $(notdir  $< ) " && $(RCC) -name $(patsubst %.qrc, %, $(notdir  $< ) ) -o $@ $<
endif 


# ----- Cleanup of generated files -----

CLEANFILES = moc_*.cxx \
             ui_*.h \
             qrc_*.cpp 
            
            
METASOURCES = auto    
