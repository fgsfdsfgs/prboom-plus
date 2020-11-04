#include "utils.h"
#include "files.h"
#include "input.h"
#include "screen.h"
#include "ui.h"

void UI_MenuAudio_Init(void);
void UI_MenuAudio_Update(void);
void UI_MenuAudio_Draw(void);

static const char *sample_rates[] = { "11025", "22050", "44100", "48000" };

static const char *music_type_labels[] = { "OPL2", "SDL" };
static const char *music_type_values[] = { "opl2", "sdl" };

static const char *sound_type_labels[] = { "Digital", "PC Speaker" };
static const char *sound_type_values[] = { "0", "1" };

static struct Option audio_opts[] =
{
    {
        OPT_CHOICE,
        "Sound type",
        "snd_pcspeaker", NULL,
        .choice =
        {
            sound_type_labels, sound_type_values,
            2, 0,
        },
    },
    {
        OPT_CHOICE,
        "MIDI player",
        "snd_midiplayer", NULL,
        .choice =
        {
            music_type_labels, music_type_values,
            2, 0,
        },
    },
    {
        OPT_CHOICE,
        "Sample rate",
        "samplerate", NULL,
        .choice =
        {
            sample_rates, sample_rates,
            4, 1,
        },
    },
    {
        OPT_INTEGER,
        "Sound channels",
        "snd_channels", NULL,
        .inum = { 1, 32, 1, 32 },
    },
    {
        OPT_BOOLEAN,
        "Random pitch shifting",
        "pitched_sounds", NULL,
        .boolean = 0,
    },
};

struct Menu ui_menu_audio =
{
    MENU_AUDIO,
    "Audio",
    "Audio settings",
    audio_opts, sizeof(audio_opts) / sizeof(*audio_opts), 0, 0,
    UI_MenuAudio_Init,
    UI_MenuAudio_Update,
    UI_MenuAudio_Draw,
};

static struct Menu *self = &ui_menu_audio;

void UI_MenuAudio_Init(void)
{

}

void UI_MenuAudio_Update(void)
{

}

void UI_MenuAudio_Draw(void)
{

}
