// Lanzador por juego: abre el nucleo de Tico con el juego ya elegido.
#include <switch.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "fixed.h"

static const char *CAND[] = {
    "sdmc:/tico/cores/%s.nro",
    "sdmc:/switch/tico/cores/%s.nro",
    "sdmc:/switch/%s/%s.nro",
    "sdmc:/switch/%s.nro",
    "sdmc:/tico/%s.nro",
};

static int exists(const char *p) { struct stat st; return stat(p, &st) == 0; }

int main(int argc, char **argv) {
    char path[300], args[1024];
    const char *found = NULL;
    for (unsigned i = 0; i < sizeof(CAND) / sizeof(CAND[0]); i++) {
        snprintf(path, sizeof(path), CAND[i], FWD_CORE, FWD_CORE);
        if (exists(path)) { found = path; break; }
    }
    if (found && exists(FWD_ROM)) {
#if FWD_MODE == 0
        snprintf(args, sizeof(args), "\"%s\" %s \"%s\" \"%s\"", found, FWD_SLUG, FWD_ROM, FWD_TITLE);
#else
        snprintf(args, sizeof(args), "\"%s\" \"%s\"", found, FWD_ROM);
#endif
        Result rc = envSetNextLoad(found, args);
        if (R_SUCCEEDED(rc)) return 0;
    }
    consoleInit(NULL);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad; padInitializeDefault(&pad);
    printf("%s\n\n", FWD_TITLE);
    if (!found) {
        printf("No encontre el nucleo %s.\n", FWD_CORE);
        printf("Instalalo desde Tico: Settings > Cores.\nBusque en:\n");
        for (unsigned i = 0; i < sizeof(CAND) / sizeof(CAND[0]); i++) {
            snprintf(path, sizeof(path), CAND[i], FWD_CORE, FWD_CORE);
            printf("  %s\n", path);
        }
    } else if (!exists(FWD_ROM)) {
        printf("No encontre el juego:\n  %s\n\nRevisa que el nombre sea exacto.\n", FWD_ROM);
    } else {
        printf("No pude abrir el nucleo:\n  %s\n", found);
    }
    printf("\nPulsa + para salir.\n");
    consoleUpdate(NULL);
    while (appletMainLoop()) {
        padUpdate(&pad);
        if (padGetButtonsDown(&pad) & HidNpadButton_Plus) break;
        consoleUpdate(NULL);
    }
    consoleExit(NULL);
    return 0;
}
