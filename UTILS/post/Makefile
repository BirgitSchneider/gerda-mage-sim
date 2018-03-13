# Makefile
#
# Author: Luigi Pertoldi - luigi.pertoldi@pd.infn.it
# Created: 21/01/2017
#
# USAGE:
#  - make sim : launch the simulations in the cluster
#  - make bin : compile the command line tools
#  - make post : lauch post-processing in the cluster
#
# NOTES:
#  - use the --dry-run flag to see what would be done without
#    actually executing it (useful also in conjunction with --debug)
#
# *CHANGE ME* important variables
CYCLE          = v0.5
RUNS           = $(shell sed '3!d;s/,//g' settings/run-list.json | cut -d "[" -f2 | cut -d "]" -f1)
GERDA_MAGE_SIM = /lfs/l2/gerda/gerda-simulations/gerda-mage-sim
GERDA_PDFS     = /lfs/l2/gerda/gerda-simulations/gerda-pdfs
DEST_DIR       = $(GERDA_PDFS)/$(CYCLE)
QSUB           = $(shell which qsub)

all :
	@echo "Please pick up one of the following options:"
	@echo " - make post : lauch post-processing in the cluster"

# qsub script must accept the syntax:
# qsub job-scheduler/mpik-t4z-gen.qsub <folder-with-raw> <dest-dir-tree>
# qsub job-scheduler/mpik-pdf-gen.qsub <volume>/<part>/<isotope> <dest-dir-tree>
# qsub job-scheduler/mpik-pdf-gen-volume.qsub <volume> <dest-dir-tree>
JOBT4Z     = ../job-scheduler/mpik-t4z-gen.qsub
JOBPDF     = ../job-scheduler/mpik-pdf-gen.qsub
JOBPDFVOL  = ../job-scheduler/mpik-pdf-gen-volume.qsub

# functions to change the position of the directory tree
change_tree  = $(shell echo $(2)/$(shell echo $(1) | rev | cut -d'/' -f-4 | rev))
change_tree2 = $(shell echo $(2)/$(shell echo $(1) | rev | cut -d'/' -f-5 | rev))

# extract fields from path
volume  = $(shell echo $(1) | cut -d'/' -f3)
part    = $(shell echo $(1) | cut -d'/' -f4)
isotope = $(shell echo $(1) | cut -d'/' -f5)
deptype = $(shell echo $(1) | cut -d'/' -f6)

# list all needed pdf- files
# 'sort' will remove duplicates
CABLES_ISOTOPES = $(sort $(foreach d,$(wildcard ../../cables/*/*/edep ../../cables/*/*/coin),$(shell dirname $(d))))
CABLES_ALL_PDF_FILES = $(foreach i,$(CABLES_ISOTOPES),$(call change_tree,$(i)/pdf-$(call volume,$(i))-$(call part,$(i))-$(call isotope,$(i)).root,$(DEST_DIR)))
CABLES_PDF_FILES = $(sort $(foreach i,$(CABLES_ISOTOPES),$(DEST_DIR)/cables/pdf-cables-$(call isotope,$(i)).root))

MINIS_ISOTOPES = $(sort $(foreach d,$(wildcard ../../minishroud/*/*/edep ../../minishroud/*/*/coin),$(shell dirname $(d))))
MINIS_ALL_PDF_FILES = $(foreach i,$(MINIS_ISOTOPES),$(call change_tree,$(i)/pdf-$(call volume,$(i))-$(call part,$(i))-$(call isotope,$(i)).root,$(DEST_DIR)))
MINIS_PDF_FILES = $(sort $(foreach i,$(MINIS_ISOTOPES),$(DEST_DIR)/minishroud/pdf-minishroud-$(call isotope,$(i)).root))

GEDET_ISOTOPES = $(sort $(foreach d,$(wildcard ../../gedet/*/*/edep ../../gedet/*/*/coin),$(shell dirname $(d))))
GEDET_ALL_PDF_FILES = $(foreach i,$(GEDET_ISOTOPES),$(call change_tree,$(i)/pdf-$(call volume,$(i))-$(call part,$(i))-$(call isotope,$(i)).root,$(DEST_DIR)))

GE_HOLDERS_ISOTOPES = $(sort $(foreach d,$(wildcard ../../ge_holders/*/*/edep ../../ge_holders/*/*/coin),$(shell dirname $(d))))
GE_HOLDERS_ALL_PDF_FILES = $(foreach i,$(GE_HOLDERS_ISOTOPES),$(call change_tree,$(i)/pdf-$(call volume,$(i))-$(call part,$(i))-$(call isotope,$(i)).root,$(DEST_DIR)))

LAR_ISOTOPES = $(sort $(foreach d,$(wildcard ../../lar/*/*/edep ../../lar/*/*/coin),$(shell dirname $(d))))
LAR_ALL_PDF_FILES = $(foreach i,$(LAR_ISOTOPES),$(call change_tree,$(i)/pdf-$(call volume,$(i))-$(call part,$(i))-$(call isotope,$(i)).root,$(DEST_DIR)))

LARVETO_ISOTOPES = $(sort $(foreach d,$(wildcard ../../larveto/*/*/edep ../../larveto/*/*/coin),$(shell dirname $(d))))
LARVETO_ALL_PDF_FILES = $(foreach i,$(LARVETO_ISOTOPES),$(call change_tree,$(i)/pdf-$(call volume,$(i))-$(call part,$(i))-$(call isotope,$(i)).root,$(DEST_DIR)))

ELECTRONICS_ISOTOPES = $(sort $(foreach d,$(wildcard ../../electronics/*/*/edep ../../electronics/*/*/coin),$(shell dirname $(d))))
ELECTRONICS_ALL_PDF_FILES = $(foreach i,$(ELECTRONICS_ISOTOPES),$(call change_tree,$(i)/pdf-$(call volume,$(i))-$(call part,$(i))-$(call isotope,$(i)).root,$(DEST_DIR)))

# function to retrieve the dependencies of the global pdf- files for cables and minishroud
# ARG1 : volume
# ARG2: isotope
get_pdf_files_for_iso = $(foreach d,$(wildcard ../../$(1)/*/$(2)),$(call change_tree,$(d)/pdf-$(1)-$(call part,$(d))-$(2).root,$(DEST_DIR)))

# function to check if a job is already in the qsub queue
# ARG1: job name
check_if_job_exists = $(shell if [[ `qstat -r | grep $(1) | grep "Full jobname:"` ]]; then echo 1; else echo 0; fi)

# final target list
post : preliminary $(CABLES_PDF_FILES) $(MINIS_PDF_FILES) $(GEDET_ALL_PDF_FILES) $(GE_HOLDERS_ALL_PDF_FILES) $(LAR_ALL_PDF_FILES) $(LARVETO_ALL_PDF_FILES) $(ELECTRONICS_ALL_PDF_FILES)

preliminary :
	@mkdir -p $(DEST_DIR)
	cp $(GERDA_MAGE_SIM)/UTILS/sim-parameters-all.json $(DEST_DIR)/sim-parameters-all.json

# this is needed to create a comma separated list for -jold_jid
space := $(null) #
comma := ,
# rule template for global pdf- files, they depend on pdf- files for each part
define rule_for_volume_pdf
  $(1) : $$(call get_pdf_files_for_iso,$(2),$$(shell echo $$(basename $(1)) | rev | cut -d'-' -f1 | rev))
	@if [[ $$(call check_if_job_exists,$$(shell echo $$(notdir $$@) | cut -d'-' -f-2)-$$(CYCLE)) == 0 ]]; then \
	$$(QSUB) -hold_jid "pdf-$(2)-*-$$(CYCLE)" \
                   -N $$(shell echo $$(notdir $$@) | cut -d'-' -f-2)-$$(CYCLE) \
                   $$(JOBPDFVOL) $$(shell echo $$@ | rev | cut -d'-' -f2 | rev) \
                   $$(DEST_DIR); \
	else \
	echo "There's already a job for $$(shell echo $$(notdir $$@) | cut -d'-' -f-2)-$$(CYCLE) in queue"; \
	fi
endef
# loop over cables and minishroud global pdf- files
$(foreach t,$(CABLES_PDF_FILES),$(eval $(call rule_for_volume_pdf,$(t),cables)))
$(foreach t,$(MINIS_PDF_FILES),$(eval $(call rule_for_volume_pdf,$(t),minishroud)))

# function to build the t4ized file name
# ARG1: directory with raw files
# ARG2: runID
t4zfile = t4z-$(call volume,$(1))-$(call part,$(1))-$(call isotope,$(1))-$(call deptype,$(1))-run$(2).root
# function to get the list of t4z- files that will be created
# ARG1: directory with raw files
get_t4z_filelist = $(foreach id,$(RUNS),$(call change_tree2,$(1)/$(call t4zfile,$(1),$(id)),$(DEST_DIR)))

# rule template for a single pdf- file, it depends on the corresponding t4z- files
define rule_for_single_pdf
  $(1) : $$(foreach d,$$(wildcard $$(call change_tree,$$(dir $(1)),../..)edep $$(call change_tree,$$(dir $(1)),../..)coin),$$(call get_t4z_filelist,$$(d)))
	@if [[ $$(call check_if_job_exists,$$(notdir $$@)-$$(CYCLE)) == 0 ]]; then \
	$$(QSUB) -hold_jid t4z-$$(shell echo $$(notdir $$(basename $$@)) | cut -d'-' -f2-)-edep-$$(CYCLE),t4z-$$(shell echo $$(notdir $$(basename $$@)) | cut -d'-' -f2-)-coin-$$(CYCLE) \
                   -N $$(notdir $$@)-$$(CYCLE) \
                   $$(JOBPDF) $$(shell echo $$@ | rev | cut -d'-' -f3 | rev)/$$(shell echo $$@ | rev | cut -d'-' -f2 | rev)/$$(shell echo $$(basename $$@) | rev | cut -d'-' -f1 | rev) \
                   $$(DEST_DIR); \
	else \
	echo "There's already a job for $$(notdir $$@)-$$(CYCLE) in queue"; \
	fi
endef
# loop over all pdf- files
ISOTOPES = $(sort $(foreach d,$(wildcard ../../*/*/*/edep ../../*/*/*/coin),$(shell dirname $(d))))
ALL_PDF_FILES = $(foreach i,$(ISOTOPES),$(call change_tree,$(i)/pdf-$(call volume,$(i))-$(call part,$(i))-$(call isotope,$(i)).root,$(DEST_DIR)))
$(foreach t,$(ALL_PDF_FILES),$(eval $(call rule_for_single_pdf,$(t))))

# rule template for the single t4z- files, it depends on the corresponding raw- files
define rule_for_single_t4z
  $(1) : $$(wildcard $$(call change_tree2,$$(dir $(1)),$$(GERDA_MAGE_SIM))raw-*.root)
	@if [[ $$(call check_if_job_exists,$$(shell echo $$(notdir $$@) | cut -d'-' -f-5)-$$(CYCLE)) == 0 ]]; then \
	$$(QSUB) -N $$(shell echo $$(notdir $$@) | cut -d'-' -f-5)-$$(CYCLE) \
                    $$(JOBT4Z) $$(call change_tree2,$$(dir $$@),$$(GERDA_MAGE_SIM)) $$(DEST_DIR); \
	else \
	echo "There's already a $$(shell echo $$(notdir $$@) | cut -d'-' -f-5)-$$(CYCLE) job in queue"; \
	fi
endef
# loop over all t4z- files
ALL_DIRS_WITH_RAW = $(wildcard ../../*/*/*/edep ../../*/*/*/coin)
ALL_T4Z_FILES = $(foreach d,$(ALL_DIRS_WITH_RAW),$(call get_t4z_filelist,$(d)))
$(foreach t,$(ALL_T4Z_FILES),$(eval $(call rule_for_single_t4z,$(t))))

.PHONY : post mkdir
