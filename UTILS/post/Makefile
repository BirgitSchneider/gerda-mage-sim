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
CYCLE          = v1.0
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
JOBT4Z     = $(GERDA_MAGE_SIM)/UTILS/job-scheduler/mpik-t4z-gen.qsub
JOBPDF     = $(GERDA_MAGE_SIM)/UTILS/job-scheduler/mpik-pdf-gen.qsub
JOBPDFVOL  = $(GERDA_MAGE_SIM)/UTILS/job-scheduler/mpik-pdf-gen-volume.qsub

# functions to change the position of the directory tree
change_to_gms  = $(shell echo $(GERDA_MAGE_SIM)/$(shell echo $(1) | awk -F '$(DEST_DIR)/' '{print $$2}'))
change_to_gpdf = $(shell echo $(DEST_DIR)/$(shell echo $(1) | awk -F '$(GERDA_MAGE_SIM)/' '{print $$2}'))

# extract fields from gerda-mage-sim paths
volume_gms   = $(shell echo $(1) | awk -F '$(GERDA_MAGE_SIM)' '{print $$2}' | cut -d'/' -f2)
part_gms     = $(shell echo $(1) | awk -F '$(GERDA_MAGE_SIM)' '{print $$2}' | cut -d'/' -f3)
isotope_gms  = $(shell echo $(1) | awk -F '$(GERDA_MAGE_SIM)' '{print $$2}' | cut -d'/' -f4)
deptype_gms  = $(shell echo $(1) | awk -F '$(GERDA_MAGE_SIM)' '{print $$2}' | cut -d'/' -f5)
vpi_gms = $(call volume_gms,$(1))-$(call part_gms,$(1))-$(call isotope_gms,$(1))
# extract fields from dest-dir paths
volume_gpdf  = $(shell echo $(1) | awk -F '$(DEST_DIR)' '{print $$2}' | cut -d'/' -f2)
part_gpdf    = $(shell echo $(1) | awk -F '$(DEST_DIR)' '{print $$2}' | cut -d'/' -f3)
isotope_gpdf = $(shell echo $(1) | awk -F '$(DEST_DIR)' '{print $$2}' | cut -d'/' -f4)
deptype_gpdf = $(shell echo $(1) | awk -F '$(DEST_DIR)' '{print $$2}' | cut -d'/' -f5)
vpi_gpdf = $(call volume_gpdf,$(1))-$(call part_gpdf,$(1))-$(call isotope_gpdf,$(1))

# list all needed pdf- files
# 'sort' will remove duplicates
CABLES_ISOTOPES = $(sort $(foreach d,$(wildcard $(GERDA_MAGE_SIM)/cables/*/*/edep $(GERDA_MAGE_SIM)/cables/*/*/coin),$(shell dirname $(d))))
CABLES_ALL_PDF_FILES = $(foreach i,$(CABLES_ISOTOPES),$(call change_to_gpdf,$(i)/pdf-$(call volume_gms,$(i))-$(call part_gms,$(i))-$(call isotope_gms,$(i)).root))

MINIS_ISOTOPES = $(sort $(foreach d,$(wildcard $(GERDA_MAGE_SIM)/minishroud/*/*/edep $(GERDA_MAGE_SIM)/minishroud/*/*/coin),$(shell dirname $(d))))
MINIS_ALL_PDF_FILES = $(foreach i,$(MINIS_ISOTOPES),$(call change_to_gpdf,$(i)/pdf-$(call volume_gms,$(i))-$(call part_gms,$(i))-$(call isotope_gms,$(i)).root))

GEDET_ISOTOPES = $(sort $(foreach d,$(wildcard $(GERDA_MAGE_SIM)/gedet/*/*/edep $(GERDA_MAGE_SIM)/gedet/*/*/coin),$(shell dirname $(d))))
GEDET_ALL_PDF_FILES = $(foreach i,$(GEDET_ISOTOPES),$(call change_to_gpdf,$(i)/pdf-$(call volume_gms,$(i))-$(call part_gms,$(i))-$(call isotope_gms,$(i)).root))

GE_HOLDERS_ISOTOPES = $(sort $(foreach d,$(wildcard $(GERDA_MAGE_SIM)/ge_holders/*/*/edep $(GERDA_MAGE_SIM)/ge_holders/*/*/coin),$(shell dirname $(d))))
GE_HOLDERS_ALL_PDF_FILES = $(foreach i,$(GE_HOLDERS_ISOTOPES),$(call change_to_gpdf,$(i)/pdf-$(call volume_gms,$(i))-$(call part_gms,$(i))-$(call isotope_gms,$(i)).root))

LAR_ISOTOPES = $(sort $(foreach d,$(wildcard $(GERDA_MAGE_SIM)/lar/*/*/edep $(GERDA_MAGE_SIM)/lar/*/*/coin),$(shell dirname $(d))))
LAR_ALL_PDF_FILES = $(foreach i,$(LAR_ISOTOPES),$(call change_to_gpdf,$(i)/pdf-$(call volume_gms,$(i))-$(call part_gms,$(i))-$(call isotope_gms,$(i)).root))

LARVETO_ISOTOPES = $(sort $(foreach d,$(wildcard $(GERDA_MAGE_SIM)/larveto/*/*/edep $(GERDA_MAGE_SIM)/larveto/*/*/coin),$(shell dirname $(d))))
LARVETO_ALL_PDF_FILES = $(foreach i,$(LARVETO_ISOTOPES),$(call change_to_gpdf,$(i)/pdf-$(call volume_gms,$(i))-$(call part_gms,$(i))-$(call isotope_gms,$(i)).root))

ELECTRONICS_ISOTOPES = $(sort $(foreach d,$(wildcard $(GERDA_MAGE_SIM)/electronics/*/*/edep $(GERDA_MAGE_SIM)/electronics/*/*/coin),$(shell dirname $(d))))
ELECTRONICS_ALL_PDF_FILES = $(foreach i,$(ELECTRONICS_ISOTOPES),$(call change_to_gpdf,$(i)/pdf-$(call volume_gms,$(i))-$(call part_gms,$(i))-$(call isotope_gms,$(i)).root))

ALL_PDF_FILES = $(CABLES_ALL_PDF_FILES) \
                $(MINIS_ALL_PDF_FILES) \
                $(GEDET_ALL_PDF_FILES) \
                $(GE_HOLDERS_ALL_PDF_FILES) \
                $(LAR_ALL_PDF_FILES) \
                $(LARVETO_ALL_PDF_FILES) \
                $(ELECTRONICS_ALL_PDF_FILES)

# final target list
.PHONY : post
post : preliminary alphas $(ALL_PDF_FILES)

.PHONY : preliminary
preliminary :
	mkdir -p $(DEST_DIR)/prod-settings
	rsync $(GERDA_MAGE_SIM)/UTILS/sim-parameters-all.json $(DEST_DIR)/sim-parameters-all.json
	rsync $(GERDA_MAGE_SIM)/UTILS/post/settings/* $(DEST_DIR)/prod-settings/
	/lfs/l3/gerda/sturm/sw/.install/julia/linux-scientific-7.3-x86_64/0.6.2/bin/julia latex-report.jl ../sim-parameters-all.json
	rsync gerda-mage-sim-report.* $(DEST_DIR)/

# list of already processed alpha histos_ISOTOPE_(LAr)pPlus.root files to be copied
ALPHA_HIST_FILES = $(shell find $(GERDA_MAGE_SIM)/alphas/ -iname "pdf*root")

.PHONY : alphas
alphas :
	mkdir -p $(DEST_DIR)/alphas
	rsync $(ALPHA_HIST_FILES) $(DEST_DIR)/alphas/

# function to build the t4ized file name
# ARG1: directory with raw files
# ARG2: runID
t4zfile = t4z-$(call volume_gms,$(1))-$(call part_gms,$(1))-$(call isotope_gms,$(1))-$(call deptype_gms,$(1))-run$(2).root
# function to get the list of t4z- files that will be created
# ARG1: directory with raw files
get_t4z_filelist = $(foreach id,$(RUNS),$(call change_to_gpdf,$(1)/$(call t4zfile,$(1),$(id))))

# function to check if a job is already in the qsub queue
# ARG1: job name
check_if_job_exists = $(shell if [[ `qstat -r | grep $(1) | grep "Full jobname:"` ]]; then echo 1; else echo 0; fi)

.SECONDEXPANSION:
$(ALL_PDF_FILES) : %.root : $$(foreach d,$$(wildcard $$(call change_to_gms,$$(dir $$*)edep) $$(call change_to_gms,$$(dir $$*)coin)),$$(call get_t4z_filelist,$$d))
	@if [[ $(call check_if_job_exists,$(notdir $@)-$(CYCLE)) == 0 ]]; then \
	$(QSUB) -hold_jid t4z-$(call vpi_gpdf,$@)-edep-$(CYCLE),t4z-$(call vpi_gpdf,$@)-coin-$(CYCLE) \
                   -N $(notdir $@)-$(CYCLE) \
                   $(JOBPDF) $(call volume_gpdf,$@)/$(call part_gpdf,$@)/$(call isotope_gpdf,$@) \
                   $(DEST_DIR); \
	else \
	echo "There's already a job for $(notdir $@)-$(CYCLE) in queue"; \
	fi

ALL_DIRS_WITH_RAW = $(wildcard $(GERDA_MAGE_SIM)/*/*/*/edep $(GERDA_MAGE_SIM)/*/*/*/coin)
ALL_T4Z_FILES = $(foreach d,$(ALL_DIRS_WITH_RAW),$(call get_t4z_filelist,$(d)))

.SECONDEXPANSION:
$(ALL_T4Z_FILES) : % : $$(wildcard $$(call change_to_gms,$$(dir $$*))raw-*.root)
	@if [[ $(call check_if_job_exists,$(shell echo $(notdir $@) | rev | cut -d'-' -f2- | rev)-$(CYCLE)) == 0 ]]; then \
	$(QSUB) -N $(shell echo $(notdir $@) | rev | cut -d'-' -f2- | rev)-$(CYCLE) \
                    $(JOBT4Z) $(call change_to_gms,$(dir $@)) $(DEST_DIR); \
	else \
	echo "There's already a $(shell echo $(notdir $@) | rev | cut -d'-' -f2- | rev)-$(CYCLE) job in queue"; \
	fi
