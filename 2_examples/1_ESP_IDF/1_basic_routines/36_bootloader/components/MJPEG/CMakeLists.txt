list(APPEND srcs    jaricom.c
                    jcomapi.c
                    jdapistd.c
                    jdarith.c
                    jdcoefct.c
                    jdcolor.c
                    jddctmgr.c
                    jdinput.c
                    jdmainct.c
                    jdmaster.c
                    jdmerge.c
                    jdpostct.c
                    jdsample.c
                    jerror.c
                    jidctflt.c
                    jidctfst.c
                    jidctint.c
                    jmemmgr.c
                    jmemnobs.c
                    jquant1.c
                    jquant2.c
                    jutils.c
                    jdapimin.c
                    jdhuff.c
                    jdmarker.c
                    jdatasrc.c)


idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ".")
