/** @file scim_array_imengine.cpp
 * implementation of class ArrayInstance.
 */

/*
 * Array 30 Input Method Engine for SCIM
 * 
 * Copyright (c) 2007 Yu-Chun Wang <albyu35@ms57.hinet.net>
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 2 of 
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 *
 */

#define Uses_SCIM_IMENGINE
#define Uses_SCIM_ICONV
#define Uses_C_STRING
#define Uses_SCIM_CONFIG_BASE
#define Uses_SCIM_CONFIG_PATH

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <scim.h>
#include "scim_array_imengine.h"

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


#define scim_module_init array_LTX_scim_module_init
#define scim_module_exit array_LTX_scim_module_exit
#define scim_imengine_module_init array_LTX_scim_imengine_module_init
#define scim_imengine_module_create_factory array_LTX_scim_imengine_module_create_factory

#define SCIM_CONFIG_IMENGINE_ARRAY_ENCHKEY   "/IMEngine/Array/Enchkey"
#define SCIM_CONFIG_IMENGINE_ARRAY_HFKEY     "/IMEngine/Array/Hfkey"
#define SCIM_CONFIG_IMENGINE_ARRAY_SHOW_SPECIAL "/IMEngine/Array/ShowSpecial"
#define SCIM_CONFIG_IMENGINE_ARRAY_SPECIAL_CODE_ONLY "/IMEngine/Array/SpecialCodeOnly"
#define SCIM_CONFIG_IMENGINE_ARRAY_USE_PHRASES  "/IMEngine/Array/UsePhrases"

#define SCIM_PROP_STATUS                    "/IMEngine/Array/Status"
#define SCIM_PROP_LETTER                    "/IMEngine/Array/Letter"

#define SCIM_ARRAY_MAIN_CIN_TABLE           (SCIM_ARRAY_TABLEDIR "/array30.cin")
#define SCIM_ARRAY_SHORT_CODE_CIN_TABLE     (SCIM_ARRAY_TABLEDIR "/array-shortcode.cin")
#define SCIM_ARRAY_SPECIAL_CIN_TABLE        (SCIM_ARRAY_TABLEDIR "/array-special.cin")
#define SCIM_ARRAY_PHRASE_CIN_TABLE         (SCIM_ARRAY_TABLEDIR "/array-phrases.cin")
#define SCIM_ARRAY_USER_PHRASES_DIR         (SCIM_PATH_DELIM_STRING ".scim" SCIM_PATH_DELIM_STRING "Array")
#define SCIM_ARRAY_USER_PHRASES_FILE        (SCIM_PATH_DELIM_STRING ".scim" SCIM_PATH_DELIM_STRING "Array" SCIM_PATH_DELIM_STRING "phrases.cin")

#define SCIM_ARRAY_EMPTY_CHAR "⎔"

#ifndef SCIM_ARRAY_ICON_FILE
    #define SCIM_ARRAY_ICON_FILE                 (SCIM_ICONDIR "/scim-array.png")
#endif

static string valid_key_map[] = {
    "1-", //a
    "5v", //b
    "3v", //c
    "3-", //d
    "3^", //e
    "4-", //f
    "5-", //g
    "6-", //h
    "8^", //i
    "7-", //j
    "8-", //k
    "9-", //l
    "7v", //m
    "6v", //n
    "9^", //o
    "0^", //p
    "1^", //q
    "4^", //r
    "2-", //s
    "5^", //t
    "7^", //u
    "4v", //v
    "2^", //w
    "2v", //x
    "6^", //y
    "1v", //z
    "8v", //,
    "9v", //.
    "0v", ///
    "0-"  //;
};


using namespace scim;

static ConfigPointer _scim_config (0);

static Pointer <ArrayFactory> __array_factory;

extern "C" {
    void scim_module_init (void)
    {
        bindtextdomain (GETTEXT_PACKAGE, SCIM_ARRAY_LOCALEDIR);
        bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    }

    void scim_module_exit (void)
    {
        __array_factory.reset ();
        _scim_config.reset ();
    }

    unsigned int scim_imengine_module_init (const ConfigPointer &config)
    {
        _scim_config = config;

        return 1;
    }

    IMEngineFactoryPointer scim_imengine_module_create_factory (unsigned int factory)
    {
        String languages;

        if (factory != 0) return NULL;

        if (__array_factory.null ())
            __array_factory = new ArrayFactory (_scim_config);

        return __array_factory;
    }
}

// implementation of Array
ArrayFactory::ArrayFactory (const ConfigPointer &config) 
    : m_status_property (SCIM_PROP_STATUS, "English/Chinese Input"),
      m_letter_property (SCIM_PROP_LETTER, "Full/Half Letter"),
      m_use_phrases(config->read(
                  String(SCIM_CONFIG_IMENGINE_ARRAY_USE_PHRASES), false))
{
    m_config = config;

    SCIM_DEBUG_IMENGINE(2) << "Start loading scim-array module\n";

    SCIM_DEBUG_IMENGINE(2) << "scim-array: start loading array30.cin from " << SCIM_ARRAY_MAIN_CIN_TABLE << "\n";
    arrayCins[0] = new ArrayCIN((char*)SCIM_ARRAY_MAIN_CIN_TABLE);

    SCIM_DEBUG_IMENGINE(2) << "scim-array: start loading array-shortcode.cin from " << SCIM_ARRAY_SHORT_CODE_CIN_TABLE << "\n";
    arrayCins[1] = new ArrayCIN((char*)SCIM_ARRAY_SHORT_CODE_CIN_TABLE);

    SCIM_DEBUG_IMENGINE(2) << "scim-array: start loading array-special.cin from " << SCIM_ARRAY_SPECIAL_CIN_TABLE << "\n";
    arrayCins[2] = new ArrayCIN((char*)SCIM_ARRAY_SPECIAL_CIN_TABLE, true);

    SCIM_DEBUG_IMENGINE(2) << "scim-array: start loading array-phrases.cin from " << SCIM_ARRAY_PHRASE_CIN_TABLE << "\n";
    if (m_use_phrases)
    {
        arrayCins[3] = new ArrayCIN((char*)SCIM_ARRAY_PHRASE_CIN_TABLE, false, false);
        load_user_phrases();
    }
    else
        arrayCins[3] = NULL;


    m_status_property.set_tip(_("The status of the current input method. Click to change it."));
    m_letter_property.set_tip(_("The input mode of the letters. Click to toggle between half and full."));

    reload_config(config);

    m_reload_signal_connection = m_config->signal_connect_reload(slot(this, &ArrayFactory::reload_config));
}

ArrayFactory::~ArrayFactory ()
{
    m_reload_signal_connection.disconnect();

    delete arrayCins[0];
    delete arrayCins[1];
    delete arrayCins[2];
    if (m_use_phrases)
    {
        delete arrayCins[3];
        delete arrayCins[4];
    }
}

void ArrayFactory::load_user_phrases()
{
    struct stat dirstat, filestat;
    String dir_path, file_path;
    dir_path = scim_get_home_dir() + SCIM_ARRAY_USER_PHRASES_DIR;
    stat(dir_path.c_str(), &dirstat);
    if (S_ISDIR(dirstat.st_mode))
    {
        file_path = scim_get_home_dir() + SCIM_ARRAY_USER_PHRASES_FILE;
        stat(file_path.c_str(), &filestat);
        if (S_ISREG(filestat.st_mode))
        {
            arrayCins[4] = new ArrayCIN((char*)file_path.c_str());
        }
    }
}

void
ArrayFactory::reload_config(const ConfigPointer &config)
{
    if (config.null())
        return;

    String str;
    str = config->read(String(SCIM_CONFIG_IMENGINE_ARRAY_ENCHKEY),
                    String(""));
    scim_string_to_key_list(m_ench_key, str);

    str = config->read(String(SCIM_CONFIG_IMENGINE_ARRAY_HFKEY),
                    String("Shift+space"));
    scim_string_to_key_list(m_full_half_key, str);

    m_show_special = config->read(String(SCIM_CONFIG_IMENGINE_ARRAY_SHOW_SPECIAL),
                    false);

    m_special_code_only = config->read(String(SCIM_CONFIG_IMENGINE_ARRAY_SPECIAL_CODE_ONLY),
                    false);
}

WideString
ArrayFactory::get_name () const
{
    return utf8_mbstowcs (_("Array"));
}

WideString
ArrayFactory::get_authors () const
{
    return utf8_mbstowcs (String (
                "2007 Yu-Chun Wang <albyu35@ms57.hinet.net>"));
}

WideString
ArrayFactory::get_credits () const
{
    return WideString();
}

WideString
ArrayFactory::get_help () const
{
    String msg;

    String enchkey;
    String hfkey;

    scim_key_list_to_string(enchkey, m_ench_key);
    scim_key_list_to_string(hfkey, m_full_half_key);

    msg = _("SCIM Array 30 Input Method Engine ");
    msg += _("Ver.");
    msg += PACKAGE_VERSION;
    msg += "\n\n";
    msg += _("Official web site: ");
    msg += "http://scimarray.openfoundry.org\n\n\n";

    msg += _("Hot Keys");
    msg += ":\n\n";
    msg += _("En/Ch Mode Switch Key");
    msg += ": ";
    msg += enchkey;
    msg += "\n";
    msg += _("Half/Full Width Mode Switch Key");
    msg += ": ";
    msg += hfkey;
    msg += "\n\n";
    msg += _("Options");
    msg += ":\n\n";
    msg += _("Show Special Code");
    msg += ": ";
    if (m_show_special)
        msg += _("Enable");
    else
        msg += _("Disable");
    msg += "\n";
    msg += _("Only Special Code Input Mode");
    msg += ": ";
    if (m_special_code_only)
        msg += _("Enable");
    else
        msg += _("Disable");
    msg += "\n";
    msg += _("Use Phrase Library");
    msg += ": ";
    if (m_use_phrases)
        msg += _("Enable");
    else
        msg += _("Disable");

    return utf8_mbstowcs(msg);
}

String
ArrayFactory::get_uuid () const
{
    return String ("fcba2b5b-7a5d-471b-b6fa-21ec0cf7d98e");
}

String
ArrayFactory::get_icon_file () const
{
    return String (SCIM_ARRAY_ICON_FILE);
}

String
ArrayFactory::get_language () const
{
    return scim_validate_language ("zh_TW");
}

IMEngineInstancePointer
ArrayFactory::create_instance (const String& encoding, int id)
{
    return new ArrayInstance (this, encoding, id);
}

int
ArrayFactory::get_maxlen (const String &encoding)
{
    return 5;
}

// implementation of ArrayInstance
ArrayInstance::ArrayInstance (ArrayFactory *factory,
                                  const String& encoding,
                                  int id)
    : IMEngineInstanceBase (factory, encoding, id),
      m_factory (factory),
      m_use_phrases(factory->m_use_phrases),
      m_show_special(factory->m_show_special),
      m_special_code_only(factory->m_special_code_only)
{
    m_lookup_table.fix_page_size(true);

    m_max_preedit_len = m_factory->get_maxlen (encoding);

    commit_press_count = 0;

    m_forward = false;
    m_full_width_letter = false;
}

ArrayInstance::~ArrayInstance ()
{
}

inline bool hasWildcard(const WideString preedit)
{
    for (int i = 0; i < preedit.size(); i++)
    {
        ucs4_t ch = preedit[i];
        if (ch == '*' || ch == '?')
            return true;
    }
    return false;
}

bool
ArrayInstance::process_key_event (const KeyEvent& rawkey)
{
    KeyEvent key = rawkey.map_to_layout(SCIM_KEYBOARD_Default);

    if (key.is_key_release ()) return false;

    // English/Chinese change mode key
    if ( match_key_event(m_factory->m_ench_key, key))
    {
        trigger_property(SCIM_PROP_STATUS);
        return true;
    }

    // Full/Half width chang mode key
    if ( match_key_event(m_factory->m_full_half_key, key))
    {
        trigger_property(SCIM_PROP_LETTER);
        return true;
    }

    // if in forward mode
    if (m_forward)
    {
        if ( key.code >= SCIM_KEY_space && key.code <= SCIM_KEY_asciitilde)
        {
            if (m_full_width_letter)
            {
                char widthc = key.get_ascii_code();
                WideString outws;
                outws.push_back(scim_wchar_to_full_width(widthc));
                commit_string(outws);
                return true;
            }
            else
                return false;
        }
        else 
            return false;
    }

    //reset key
    if (key.code == SCIM_KEY_Escape && key.mask == 0) {
        // Do not catch ESC while no key input for VI users
        if (m_preedit_string.size() == 0)
            return false;
        reset ();
        return true;
    }

    //delete key
    if (key.code == SCIM_KEY_BackSpace && key.mask == 0 &&
        m_preedit_string.size () != 0) 
    {
        m_preedit_string.erase (m_preedit_string.length () - 1, 1);
        pre_update_preedit_string (m_preedit_string);
        process_preedit_string ();
        commit_press_count = 0;     // reset to 0 to avoid output error
        return true;
    }

    // valid keys
    if (((key.code >= SCIM_KEY_a && key.code <= SCIM_KEY_z) ||
         (key.code == SCIM_KEY_comma) || (key.code == SCIM_KEY_period) ||
         (key.code == SCIM_KEY_semicolon) || (key.code == SCIM_KEY_slash))
            &&
        (key.mask == 0))
    {
        if (m_preedit_string.length () >=  m_max_preedit_len)
            return true;
 
        if (commit_press_count == 1)
        {
            WideString str = m_lookup_table.get_candidate_in_current_page (0);
            if (str.length() && str.compare(utf8_mbstowcs(SCIM_ARRAY_EMPTY_CHAR)) != 0 ) {
                send_commit_string(m_preedit_string,
                        m_lookup_table.get_candidate_in_current_page(0));
            }
            else
                reset();
        }

        if (m_preedit_string.length () == 0)
        {
            hide_aux_string ();
            show_preedit_string ();
        }

        ucs4_t ascii = (ucs4_t) tolower (key.get_ascii_code ());
        m_preedit_string.push_back (ascii);
        pre_update_preedit_string (m_preedit_string);
        process_preedit_string ();

        return true;
    }

    // apostrophe key for end of phrases
    if (m_use_phrases && key.code == SCIM_KEY_apostrophe 
            && m_preedit_string.length())
    {
        phrase_key_press();
        return true;
    }
    
    // wildcard keys
    if (key.code == SCIM_KEY_question || key.code == SCIM_KEY_asterisk)
    {
        hide_lookup_table();
        ucs4_t ascii = (ucs4_t) tolower (key.get_ascii_code ());
        m_preedit_string.push_back (ascii);
        pre_update_preedit_string (m_preedit_string);
        process_preedit_string ();
        return true;
    }

    // selection keys
    if (key.code >= SCIM_KEY_0 && key.code <= SCIM_KEY_9) 
    {
        if (!m_preedit_string.length())
            return false;

        WideString inkey = m_preedit_string;

        // key "w" to enable the symbol input
        if (m_preedit_string.compare(utf8_mbstowcs("w")) == 0)
        {
            ucs4_t ascii = (ucs4_t) tolower (key.get_ascii_code ());
            m_preedit_string.push_back (ascii);
            pre_update_preedit_string (m_preedit_string);
            process_symbol_preedit_string ();

            return true;
        }

        int selectnum = (int)(key.get_ascii_code() - '0' - 1);
        if (selectnum < 0)
            selectnum = 9;


        WideString cmtstr = m_lookup_table.get_candidate_in_current_page(selectnum);

        if (cmtstr.length() && cmtstr.compare(utf8_mbstowcs(SCIM_ARRAY_EMPTY_CHAR)) != 0)
        {
            send_commit_string(inkey, cmtstr);
            return true;
        }
        else
        {
            return true;
        }

    }

    // commit key
    if (key.code == SCIM_KEY_space && key.mask == 0 && m_preedit_string.length ())
    {
        space_key_press();
        return true;
    }

    //page up key.
    if (key.code == SCIM_KEY_Page_Up && key.mask == 0)
        lookup_table_page_up ();

    //page down key.
    if (key.code == SCIM_KEY_Page_Down && key.mask == 0)
        lookup_table_page_down ();

    //other keys is not allowed when preediting
    if (m_preedit_string.length ())
        return true;

    //other keys wiil be send out if the Full width mode on
    if ( (key.code >= SCIM_KEY_space && key.code <= SCIM_KEY_asciitilde) && (key.mask == 0 || key.is_shift_down()))
    {
        WideString outws;
        char widthc = key.get_ascii_code();

        if (widthc >= 'A' && widthc <= 'Z')
            if (!key.is_caps_lock_down())
                widthc = widthc - 'A' + 'a';

        if (m_full_width_letter)
            outws.push_back(scim_wchar_to_full_width(widthc));
        else
            outws.push_back(widthc);

        commit_string(outws);
        return true;
    }

    return false;
}

void
ArrayInstance::space_key_press()
{
    // space is the page down key when keying symbols and phrases
    if (m_lookup_table.number_of_candidates() > m_lookup_table.get_page_size())
    {
        lookup_table_page_down();
        return;
    }

    WideString inkey = m_preedit_string;
    
    // If the user is already press the space before, 
    // commit the first candidate in the lookup table
    if (commit_press_count == 1)
    {
        WideString str = m_lookup_table.get_candidate_in_current_page (0);
        if (str.compare(utf8_mbstowcs(SCIM_ARRAY_EMPTY_CHAR)) == 0) {
            hide_lookup_table();
            return;
        }
        if (str.length()) {
            send_commit_string(inkey, str);
            return;
        }
    }

    create_lookup_table(_ScimArray::Array_Table);
    update_lookup_table(m_lookup_table);
    if (m_lookup_table.number_of_candidates() > 1)
    {
        show_lookup_table();
        commit_press_count++;
        return;
    }
    hide_lookup_table();
    WideString str = m_lookup_table.get_candidate_in_current_page (0);
    if (str.length() && str.compare(utf8_mbstowcs(SCIM_ARRAY_EMPTY_CHAR))) {
        send_commit_string(inkey, str);
        return;
    }
}

void
ArrayInstance::send_commit_string(const WideString& inkey, const WideString& str)
{
    // phrases, no need to check specil codes
    if (str.length() > 1)
    {
        commit_string(str);
        reset();
        return;
    }
    if (m_special_code_only)
    {
        if (!check_special_code_only(inkey, str))
        {
            show_special_code(inkey, str);
            return ;
        }
    }
    commit_string (str);
    reset ();
    show_special_code(inkey, str);
}

bool
ArrayInstance::check_special_code_only(const WideString& keys, const WideString& character)
{
    String skeys(utf8_wcstombs(keys));
    String scharacter(utf8_wcstombs(character));

    vector<String> outvec;
    if (m_factory->arrayCins[2]->getWordsVector(scharacter, outvec))
    {
        String spcode = outvec[0];

        // if the input keys does not match the special code
        if (spcode.compare(skeys) != 0)
        {
            return false;
        }
    }

     // if the character has no special code,
    // return true
    return true;
}

void
ArrayInstance::select_candidate (unsigned int item)
{
    WideString label = m_lookup_table.get_candidate_label (item);
    KeyEvent key ((int) label [0], 0);
    process_key_event (key);
}

void
ArrayInstance::update_lookup_table_page_size (unsigned int page_size)
{
    if (page_size > 0)
        m_lookup_table.set_page_size (page_size);
}

void
ArrayInstance::lookup_table_page_up ()
{
    if (m_preedit_string.length () && m_lookup_table.number_of_candidates ()) {
        m_lookup_table.page_up ();

        /*m_lookup_table.set_candidate_labels (
            std::vector <WideString> (
                m_lookup_table_labels.begin () + m_lookup_table.get_current_page_start (),
                m_lookup_table_labels.end ()));*/
        std::vector<WideString> labels_vec;
        create_lookup_table_labels(m_lookup_table.get_current_page_size());
        m_lookup_table.set_candidate_labels (m_lookup_table_labels);

        update_lookup_table (m_lookup_table);
    }
}

void
ArrayInstance::lookup_table_page_down ()
{
    if (m_preedit_string.length () && m_lookup_table.number_of_candidates ()) {

        if (!m_lookup_table.page_down ())
            while (m_lookup_table.page_up()) ;

        /*m_lookup_table.set_candidate_labels (
            std::vector <WideString> (
                m_lookup_table_labels.begin () + m_lookup_table.get_current_page_start (),
                m_lookup_table_labels.end ()));*/
        std::vector<WideString> labels_vec;
        create_lookup_table_labels(m_lookup_table.get_current_page_size());
        m_lookup_table.set_candidate_labels (m_lookup_table_labels);

        update_lookup_table (m_lookup_table);
    }
}

void
ArrayInstance::move_preedit_caret (unsigned int /*pos*/)
{
}

void
ArrayInstance::reset ()
{
    m_preedit_string = WideString ();
    m_aux_string = WideString ();

    m_lookup_table.clear ();
    m_lookup_table.fix_page_size(true);

    commit_press_count = 0;

    hide_lookup_table ();
    hide_preedit_string ();
    hide_aux_string ();
}

inline static string 
key_to_keyname(const char c)
{
    if (c >= 'a' && c <= 'z')
        return  valid_key_map[c - 'a'];
    else if (c == ',')
        return valid_key_map[26];
    else if (c == '.')
        return valid_key_map[27];
    else if (c == '/')
        return valid_key_map[28];
    else if (c == ';')
        return valid_key_map[29];
    else if (c == '?')
        return "?";
    else if (c == '*')
        return "*";

    return "";
}

void
ArrayInstance::pre_update_preedit_string(const WideString& in_string)
{
    string preedit = utf8_wcstombs(in_string);

    string show_preedit_string;

    for (int i = 0; i < preedit.length(); i++)
    {
        char c = preedit[i];
        show_preedit_string += key_to_keyname(c);
    }

    WideString output_string = utf8_mbstowcs(show_preedit_string);

    update_preedit_string(output_string);
    update_preedit_caret (output_string.length());
}


void
ArrayInstance::focus_in ()
{
    initialize_properties ();

    if (m_preedit_string.length ()) {
        pre_update_preedit_string (m_preedit_string);
        show_preedit_string ();
        if (m_lookup_table.number_of_candidates ()) {
            update_lookup_table (m_lookup_table);
            show_lookup_table ();
        }
    }
}

void
ArrayInstance::focus_out ()
{
    reset ();
}

void
ArrayInstance::trigger_property (const String &property)
{
    if (property == SCIM_PROP_STATUS)
    {
        m_forward = !m_forward;

        refresh_status_property();
        reset();
    }
    else if (property == SCIM_PROP_LETTER)
    {
        m_full_width_letter = !m_full_width_letter;

        refresh_letter_property();
    }
}

void
ArrayInstance::initialize_properties ()
{
    PropertyList proplist;

    proplist.push_back(m_factory->m_status_property);
    proplist.push_back(m_factory->m_letter_property);

    register_properties (proplist);

    refresh_status_property();
    refresh_letter_property();
}

void
ArrayInstance::refresh_status_property ()
{
    if (m_forward)
        m_factory->m_status_property.set_label (_("En"));
    else
        m_factory->m_status_property.set_label (_("Ch"));
    
    update_property (m_factory->m_status_property);
}
                                                                    

void
ArrayInstance::refresh_letter_property ()
{
    if (m_full_width_letter)
    {
        m_factory->m_letter_property.set_label (_("Full"));
    }
    else
    {
        m_factory->m_letter_property.set_label (_("Half"));
    }

    update_property (m_factory->m_letter_property);
}

bool
ArrayInstance::match_key_event (const KeyEventList &keys, const KeyEvent &key) const
{
    KeyEventList::const_iterator kit; 
    
    for (kit = keys.begin (); kit != keys.end (); ++kit) {
        if (!key.is_key_release()) {
            if (key.code == kit->code) {
                int mask = key.mask;        // we should ignore capslock and numlock
                mask &= ~SCIM_KEY_CapsLockMask;
                mask &= ~SCIM_KEY_NumLockMask;
                if (mask == kit->mask)
                    return true;
            }
        }
    }
    return false;
}


void
ArrayInstance::process_symbol_preedit_string()
{
    if (m_preedit_string.length () == 0) {
        hide_preedit_string ();
        hide_lookup_table ();
        return;
    }

    create_lookup_table(_ScimArray::Array_Table);

    update_lookup_table(m_lookup_table);

    show_lookup_table();
}

void
ArrayInstance::process_preedit_string ()
{
    if (m_preedit_string.length () == 0) {
        hide_preedit_string ();
        hide_lookup_table ();
        return;
    }

    if (hasWildcard(m_preedit_string))
    {
        hide_lookup_table();
        return;
    }

    if (m_preedit_string.length() <= 2)
    {
        create_lookup_table(_ScimArray::Array_Short);
        show_pre_special_code(m_preedit_string);
    }
    else
    {
        create_lookup_table(_ScimArray::Array_Table);
        hide_aux_string();
    }

    update_lookup_table(m_lookup_table);

    if (m_lookup_table.number_of_candidates ())
        show_lookup_table ();
    else
        hide_lookup_table ();
}

void 
ArrayInstance::phrase_key_press()
{
    if (m_preedit_string.length () == 0) {
        hide_preedit_string ();
        hide_lookup_table ();
        return;
    }
    create_phrase_lookup_table();
    hide_aux_string();
    update_lookup_table(m_lookup_table);
    if (m_lookup_table.number_of_candidates ())
    {
        commit_press_count++;
        show_lookup_table();
    }
    else
        hide_lookup_table();
}

inline static int int_to_ascii (int hex)
{
    hex %= 10;
    return hex + '0';
}


int
ArrayInstance::create_lookup_table (int mapSelect)
{
    String mbs_code;
    ucs4_t ucs_code;
    WideString trail;
    WideString wstr;

    m_lookup_table.clear ();
    m_lookup_table_labels.clear ();

    trail.push_back(0x20);

    bool isHaveWildcard = false;
    isHaveWildcard = hasWildcard(m_preedit_string);

    vector<string> candidatesVec;
    int rcount = 0;

    if (isHaveWildcard)
    {
        rcount = m_factory->arrayCins[mapSelect]->getWordsVectorWithWildcard(
                utf8_wcstombs(m_preedit_string), candidatesVec);
    }
    else
    {
        rcount = m_factory->arrayCins[mapSelect]->getWordsVector(
                utf8_wcstombs(m_preedit_string), candidatesVec);
    }

    if (rcount)
    {
        for (int i = 0; i < candidatesVec.size(); i++)
        {
            trail [0] = (ucs4_t) int_to_ascii ((i % 10) + 1);
            if ((i % 10) >= 9)
                trail [0] = (ucs4_t) int_to_ascii (0);

            m_lookup_table.append_candidate(utf8_mbstowcs(candidatesVec[i]));
            m_lookup_table_labels.push_back(trail);
        }
    }
    else
    {
        trail [0] = (ucs4_t) int_to_ascii (0);
        m_lookup_table.append_candidate(utf8_mbstowcs(SCIM_ARRAY_EMPTY_CHAR));
        m_lookup_table_labels.push_back(trail);
    }

    if (m_lookup_table_labels.size() <= 10)
        m_lookup_table.set_page_size (m_lookup_table_labels.size());
    else
        m_lookup_table.set_page_size(10);

    if (mapSelect == _ScimArray::Array_Phrases)
    {
        m_lookup_table.fix_page_size(false);
    }

    create_lookup_table_labels(m_lookup_table.get_current_page_size());
    m_lookup_table.set_candidate_labels (m_lookup_table_labels);

    return m_lookup_table_labels.size ();
}

int
ArrayInstance::create_phrase_lookup_table()
{
    String mbs_code;
    ucs4_t ucs_code;
    WideString trail;
    WideString wstr;

    m_lookup_table.clear ();
    m_lookup_table_labels.clear ();

    vector<string> candidatesVec, pVec;
    int rcount = 0;

    if (m_factory->arrayCins[4] != NULL)
    {
        rcount = m_factory->arrayCins[_ScimArray::Array_UserPhrases]->getWordsVector(
            utf8_wcstombs(m_preedit_string), candidatesVec);
        rcount = m_factory->arrayCins[_ScimArray::Array_Phrases]->getWordsVector(
                utf8_wcstombs(m_preedit_string), pVec);
        vector<string>::iterator itr;
        for(itr = pVec.begin(); itr != pVec.end(); itr++)
            candidatesVec.push_back((*itr));
    }
    else
    {
        rcount = m_factory->arrayCins[3]->getWordsVector(
                utf8_wcstombs(m_preedit_string), candidatesVec);
    }
    if (candidatesVec.size())
    {
        for (int i = 0; i < candidatesVec.size(); i++)
        {
            trail [0] = (ucs4_t) int_to_ascii ((i % 10) + 1);
            if ((i % 10) >= 9)
                trail [0] = (ucs4_t) int_to_ascii (0);
            m_lookup_table.append_candidate(utf8_mbstowcs(candidatesVec[i]));
            m_lookup_table_labels.push_back(trail);
        }
    }
    else
    {
        trail [0] = (ucs4_t) int_to_ascii (0);
        m_lookup_table.append_candidate(utf8_mbstowcs(SCIM_ARRAY_EMPTY_CHAR));
        m_lookup_table_labels.push_back(trail);
    }
    m_lookup_table.set_page_size(10);
    m_lookup_table.fix_page_size(false);
    create_lookup_table_labels(m_lookup_table.get_current_page_size());
    m_lookup_table.set_candidate_labels (m_lookup_table_labels);
    return m_lookup_table_labels.size ();
}

void
ArrayInstance::create_lookup_table_labels(int page_size) 
{
    WideString trail;
    trail.push_back(0x20);
    m_lookup_table_labels.clear();
    for (int i = 0; i < page_size; i++)
    {
        trail [0] = (ucs4_t) int_to_ascii ((i % 10) + 1);
        if ((i % 10) >= 9)
            trail [0] = (ucs4_t) int_to_ascii (0);
        m_lookup_table_labels.push_back(trail);
    }
}

bool
ArrayInstance::show_special_code(const WideString& inkey, const WideString& spchar)
{
    if (!m_show_special)
    {
        hide_aux_string();
        return false;
    }

    m_aux_string = WideString();

    vector<string> candidatesVec;

    if (m_factory->arrayCins[2]->getWordsVector(
                utf8_wcstombs(spchar), candidatesVec))
    {
        string keys = candidatesVec[0];
        string sinkeys = utf8_wcstombs(inkey);

        // If the user already inputs the special code,
        // we don't have to note that
        if (!keys.compare(sinkeys))
            return false;

        m_aux_string += utf8_mbstowcs("「");
        m_aux_string += spchar;
        m_aux_string += utf8_mbstowcs("」");
        m_aux_string += utf8_mbstowcs(_("special code:"));

        for (int i = 0; i < keys.length(); i++)
        {
            char c = keys[i];
            m_aux_string += utf8_mbstowcs(key_to_keyname(c));
        }

        update_aux_string(m_aux_string);

        show_aux_string();

        return true;
    }
    else
    {
        hide_aux_string();
        return false;
    }
}

bool
ArrayInstance::show_pre_special_code(const WideString& inkey)
{
    if (!m_show_special)
    {
        hide_aux_string();
        return false;
    }

    m_aux_string = WideString();

    vector<string> candidatesVec;

    if (m_factory->arrayCins[2]->getReverseWordsVector(
                utf8_wcstombs(inkey), candidatesVec))
    {
        m_aux_string += utf8_mbstowcs("「");
        m_aux_string += utf8_mbstowcs(candidatesVec[0]);
        m_aux_string += utf8_mbstowcs("」");
        m_aux_string += utf8_mbstowcs(_("special code:"));

        string keys = utf8_wcstombs(inkey);
        for (int i = 0; i < keys.length(); i++)
        {
            char c = keys[i];
            m_aux_string += utf8_mbstowcs(key_to_keyname(c));
        }

        update_aux_string(m_aux_string);

        show_aux_string();

        return true;
    }
    else
    {
        hide_aux_string();
        return false;
    }
}

/*
vi:ts=4:nowrap:ai:expandtab
*/
