/** @file scim_array_imengine_setup.cpp
 * implementation of Setup Module of Array imengine module.
 */

/*
 * Array 30 Input Method Engine for SCIM
 * 
 * Copyright (c) 2007 Yu-Chun Wang <albyu35@ms57.hinet.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *
 * $Id: scim_hangul_imengine_setup.cpp,v 1.8 2006/10/23 12:42:47 hwanjin Exp $
 *
 */

#include <cstring> // for gcc 4.3 header file dependency problem

#define Uses_SCIM_CONFIG_BASE

#include <gtk/gtk.h>

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <scim.h>
#include <gtk/scimkeyselection.h>

#ifdef HAVE_GETTEXT
  #include <libintl.h>
  #define _(String) dgettext(GETTEXT_PACKAGE,String)
  #define N_(String) (String)
#else
  #define _(String) (String)
  #define N_(String) (String)
  #define bindtextdomain(Package,Directory)
  #define textdomain(domain)
  #define bind_textdomain_codeset(domain,codeset)
#endif

#if GTK_CHECK_VERSION(2, 12, 0)
#else
  #define SCIM_ARRAY_USE_GTK_TOOLTIPS
#endif

#if GTK_CHECK_VERSION(3, 0, 0)
  #define SCIM_ARRAY_USE_GTK_BOX
#else
#endif

using namespace scim;

#define scim_module_init array_imengine_setup_LTX_scim_module_init
#define scim_module_exit array_imengine_setup_LTX_scim_module_exit

#define scim_setup_module_create_ui       array_imengine_setup_LTX_scim_setup_module_create_ui
#define scim_setup_module_get_category    array_imengine_setup_LTX_scim_setup_module_get_category
#define scim_setup_module_get_name        array_imengine_setup_LTX_scim_setup_module_get_name
#define scim_setup_module_get_description array_imengine_setup_LTX_scim_setup_module_get_description
#define scim_setup_module_load_config     array_imengine_setup_LTX_scim_setup_module_load_config
#define scim_setup_module_save_config     array_imengine_setup_LTX_scim_setup_module_save_config
#define scim_setup_module_query_changed   array_imengine_setup_LTX_scim_setup_module_query_changed


#define SCIM_CONFIG_IMENGINE_ARRAY_ENCHKEY    "/IMEngine/Array/Enchkey"
#define SCIM_CONFIG_IMENGINE_ARRAY_HFKEY      "/IMEngine/Array/Hfkey"
#define SCIM_CONFIG_IMENGINE_ARRAY_SHOW_SPECIAL "/IMEngine/Array/ShowSpecial"
#define SCIM_CONFIG_IMENGINE_ARRAY_SPECIAL_CODE_ONLY "/IMEngine/Array/SpecialCodeOnly"
#define SCIM_CONFIG_IMENGINE_ARRAY_USE_PHRASES "/IMEngine/Array/UsePhrases"


static GtkWidget * create_setup_window ();
static void        load_config (const ConfigPointer &config);
static void        save_config (const ConfigPointer &config);
static bool        query_changed ();

// Module Interface.
extern "C" {
    void scim_module_init (void)
    {
        bindtextdomain (GETTEXT_PACKAGE, SCIM_ARRAY_LOCALEDIR);
        bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    }

    void scim_module_exit (void)
    {
    }

    GtkWidget * scim_setup_module_create_ui (void)
    {
        static GtkWidget *setup_ui = NULL;
        if (setup_ui == NULL)
            setup_ui = create_setup_window ();
        return setup_ui;
    }

    String scim_setup_module_get_category (void)
    {
        return String ("IMEngine");
    }

    String scim_setup_module_get_name (void)
    {
        return String (_("Array"));
    }

    String scim_setup_module_get_description (void)
    {
        return String (_("Array 30 IMEngine Module."));
    }

    void scim_setup_module_load_config (const ConfigPointer &config)
    {
        load_config (config);
    }

    void scim_setup_module_save_config (const ConfigPointer &config)
    {
        save_config (config);
    }

    bool scim_setup_module_query_changed ()
    {
        return query_changed ();
    }
} // extern "C"

static GtkWidget *ec_change_key_text = NULL;
static GtkWidget *hf_change_key_text = NULL;
static GtkWidget *show_special_code_button = NULL;
static GtkWidget *special_code_only_button = NULL;
static GtkWidget *phrases_library_button = NULL;

static bool __have_changed                 = false;


// Declaration of internal functions.
static void
on_default_editable_changed          (GtkEditable     *editable,
                                      gpointer         user_data);

static void
on_default_toggle_button_toggled     (GtkToggleButton *togglebutton,
                                      gpointer         user_data);

static void
on_default_key_selection_clicked     (GtkButton       *button,
                                      gpointer         user_data);

static GtkWidget *
#ifdef SCIM_ARRAY_USE_GTK_TOOLTIPS
create_options_page(GtkTooltips *tooltip);
#else
create_options_page();
#endif


// Function implementations.
static GtkWidget *
#ifdef SCIM_ARRAY_USE_GTK_TOOLTIPS
create_options_page(GtkTooltips *tooltips)
#else
create_options_page()
#endif
{
    GtkWidget *vbox;
    GtkWidget *button;

#ifdef SCIM_ARRAY_USE_GTK_BOX
    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
#else
    vbox = gtk_vbox_new (FALSE, 12);
#endif
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 12);

    button = gtk_check_button_new_with_mnemonic (_("Show Special Code"));
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
#ifdef SCIM_ARRAY_USE_GTK_TOOLTIPS
    gtk_tooltips_set_tip(tooltips, button,
                          _("To notify if the character has the special code"), NULL);
#else
    gtk_widget_set_tooltip_text(button,
                          _("To notify if the character has the special code"));
#endif
    g_signal_connect(G_OBJECT(button), "toggled",
                     G_CALLBACK(on_default_toggle_button_toggled), NULL);
    show_special_code_button = button;

    button = gtk_check_button_new_with_mnemonic(_("Only Special Code Input Mode"));
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
#ifdef SCIM_ARRAY_USE_GTK_TOOLTIPS
    gtk_tooltips_set_tip(tooltips, button,
                            _("If the character has the special code, you will be confined to use it"), NULL);
#else
    gtk_widget_set_tooltip_text(button,
                            _("If the character has the special code, you will be confined to use it"));
#endif
    g_signal_connect(G_OBJECT(button), "toggled",
                     G_CALLBACK(on_default_toggle_button_toggled), NULL);
    special_code_only_button = button;

    button = gtk_check_button_new_with_mnemonic(_("Use Phrase Library"));
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
#ifdef SCIM_ARRAY_USE_GTK_TOOLTIPS
    gtk_tooltips_set_tip(tooltips, button,
            _("Turn on phrase input mode. (Enable after restart)"), NULL);
#else
    gtk_widget_set_tooltip_text(button,
            _("Turn on phrase input mode. (Enable after restart)"));
#endif
    g_signal_connect(G_OBJECT(button), "toggled",
                     G_CALLBACK(on_default_toggle_button_toggled), NULL);
    phrases_library_button = button;

    GtkWidget *table = gtk_table_new ( 2, 2, TRUE );
    gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, TRUE, 0);

    GtkWidget *label = gtk_label_new(_("En/Ch Mode Switch Key"));
    gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
    gtk_misc_set_padding (GTK_MISC (label), 4, 0); 
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
            (GtkAttachOptions) (GTK_FILL),
            (GtkAttachOptions) (GTK_FILL), 4, 4); 

#ifdef SCIM_ARRAY_USE_GTK_BOX
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
#else
    GtkWidget *hbox = gtk_hbox_new(FALSE, 12);
#endif

    GtkWidget *entry = gtk_entry_new();
    gtk_editable_set_editable (GTK_EDITABLE (entry), FALSE);
    gtk_entry_set_text (GTK_ENTRY (entry), "");
    gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, TRUE, 0);
    g_signal_connect(G_OBJECT(entry), "changed",
                    G_CALLBACK (on_default_editable_changed), NULL);
    ec_change_key_text = entry;

    button = gtk_button_new_with_label ("...");
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 0);
    g_signal_connect(G_OBJECT(button), "clicked",
                         G_CALLBACK (on_default_key_selection_clicked),
                         ec_change_key_text);
    gtk_table_attach_defaults(GTK_TABLE(table), hbox, 1, 2, 0, 1);

    label = gtk_label_new(_("Half/Full Width Mode Switch Key"));
    gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
    gtk_misc_set_padding (GTK_MISC (label), 4, 0); 
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2,
            (GtkAttachOptions) (GTK_FILL),
            (GtkAttachOptions) (GTK_FILL), 4, 4); 

#ifdef SCIM_ARRAY_USE_GTK_BOX
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
#else
    hbox = gtk_hbox_new(FALSE, 12);
#endif

    entry = gtk_entry_new();
    gtk_editable_set_editable (GTK_EDITABLE(entry), FALSE);
    gtk_entry_set_text (GTK_ENTRY (entry), "");
    gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, TRUE, 0);
    g_signal_connect(G_OBJECT(entry), "changed",
                    G_CALLBACK (on_default_editable_changed), NULL);
    hf_change_key_text = entry;

    button = gtk_button_new_with_label ("...");
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 0);
    gtk_table_attach_defaults(GTK_TABLE(table), hbox, 1, 2, 1, 2);
    g_signal_connect(G_OBJECT(button), "clicked",
                         G_CALLBACK (on_default_key_selection_clicked),
                         hf_change_key_text);

    return vbox;
}


static GtkWidget *
create_setup_window ()
{
    GtkWidget *notebook;
    GtkWidget *label;
    GtkWidget *page;
#ifdef SCIM_ARRAY_USE_GTK_TOOLTIPS
    GtkTooltips *tooltips;

    tooltips = gtk_tooltips_new ();
#endif

    // Create the Notebook.
    notebook = gtk_notebook_new ();

    // Create the first page.
#ifdef SCIM_ARRAY_USE_GTK_TOOLTIPS
    page = create_options_page(tooltips);
#else
    page = create_options_page();
#endif
    label = gtk_label_new (_("Options"));
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), page, label);

    gtk_notebook_set_current_page(GTK_NOTEBOOK (notebook), 0);

    gtk_widget_show_all(notebook);

    return notebook;
}

static void
load_config (const ConfigPointer &config)
{
    if (config.null())
        return;

    // English/Chinese Key
    String eckey = config->read(String(SCIM_CONFIG_IMENGINE_ARRAY_ENCHKEY), String(""));
    gtk_entry_set_text(GTK_ENTRY(ec_change_key_text), eckey.c_str());

    // Show Special Code Notice
    bool is_show_special;
    is_show_special = config->read(String(SCIM_CONFIG_IMENGINE_ARRAY_SHOW_SPECIAL), false);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(show_special_code_button), is_show_special);

    // Half/Full Width Key
    String hfkey = config->read(String(SCIM_CONFIG_IMENGINE_ARRAY_HFKEY), String("Shift+space"));
    gtk_entry_set_text(GTK_ENTRY(hf_change_key_text), hfkey.c_str());

    // Only Special Code Input Mode
    bool is_special_code_only;
    is_special_code_only = config->read(String(SCIM_CONFIG_IMENGINE_ARRAY_SPECIAL_CODE_ONLY), false);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(special_code_only_button), is_special_code_only);

    // Use Phrase Library
    bool is_use_phrase_library;
    is_use_phrase_library = config->read(
            String(SCIM_CONFIG_IMENGINE_ARRAY_USE_PHRASES), false);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(phrases_library_button),
            is_use_phrase_library);

    __have_changed = false;
}

void
save_config (const ConfigPointer &config)
{
    if (config.null())
        return;

    // English/Chinese Key
    String eckey = gtk_entry_get_text(GTK_ENTRY(ec_change_key_text));
    config->write(String(SCIM_CONFIG_IMENGINE_ARRAY_ENCHKEY), eckey);


    gboolean stat;

    // Show Special Code Notice
    stat = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(show_special_code_button));
    config->write(String(SCIM_CONFIG_IMENGINE_ARRAY_SHOW_SPECIAL), (bool)stat);

    // Only Special Code Input Mode
    stat = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(special_code_only_button));
    config->write(String(SCIM_CONFIG_IMENGINE_ARRAY_SPECIAL_CODE_ONLY), (bool)stat);

    // Use Phrase Library
    stat = gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(phrases_library_button));
    config->write(String(SCIM_CONFIG_IMENGINE_ARRAY_USE_PHRASES), 
            (bool)stat);

    // Half/Full Width Key
    String hfkey = gtk_entry_get_text(GTK_ENTRY(hf_change_key_text));
    config->write(String(SCIM_CONFIG_IMENGINE_ARRAY_HFKEY), hfkey);

    __have_changed = false;
}

bool
query_changed ()
{
    return __have_changed;
}

static void
on_default_editable_changed (GtkEditable *editable,
                             gpointer     user_data)
{
    __have_changed = true;
}

static void
on_default_toggle_button_toggled (GtkToggleButton *togglebutton,
                                  gpointer         user_data)
{
    __have_changed = true;
}


static void
on_default_key_selection_clicked (GtkButton *button,
                                  gpointer   user_data)
{
    GtkWidget *text = static_cast<GtkWidget *> (user_data);

    if (text) {
        GtkWidget *dialog = scim_key_selection_dialog_new (_("Key Event Modification"));
        gint result;

        scim_key_selection_dialog_set_keys (
            SCIM_KEY_SELECTION_DIALOG (dialog),
            gtk_entry_get_text (GTK_ENTRY (text)));

        result = gtk_dialog_run (GTK_DIALOG (dialog));

        if (result == GTK_RESPONSE_OK) {
            const gchar *keys = scim_key_selection_dialog_get_keys (
                            SCIM_KEY_SELECTION_DIALOG (dialog));

            if (!keys) keys = "";

            if (strcmp (keys, gtk_entry_get_text (GTK_ENTRY (text))) != 0)
                gtk_entry_set_text (GTK_ENTRY (text), keys);
        }

        gtk_widget_destroy (dialog);
    }
}

/*
vi:ts=4:nowrap:expandtab
*/
