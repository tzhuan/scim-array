/** @file scim_array.h
 * definition of SCIM Array related classes.
 */

/* 
 * Smart Common Input Method
 * 
 * Copyright (c) 2007 Yu-Chun Wang <albyu35@ms57.hinet.net>
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 */

#if !defined (__SCIM_ARRAY_IMENGINE_H)
#define __SCIM_ARRAY_IMENGINE_H

#include "ArrayCIN.h"

using namespace scim;

namespace _ScimArray
{
    enum EnumTable
    {
        Array_Table = 0,
        Array_Short = 1,
        Array_Special = 2,
        Array_Phrases = 3,
        Array_UserPhrases = 4
    };
};

class ArrayFactory : public IMEngineFactoryBase
{
    friend class ArrayInstance;

public:
    ArrayFactory (const ConfigPointer &config);
    virtual ~ArrayFactory ();

    virtual WideString  get_name () const;
    virtual WideString  get_authors () const;
    virtual WideString  get_credits () const;
    virtual WideString  get_help () const;
    virtual String      get_uuid () const;
    virtual String      get_icon_file () const;
    virtual String      get_language () const;

    virtual IMEngineInstancePointer create_instance (const String& encoding, int id = -1);

protected:
    ArrayCIN* arrayCins[5];
    EnumSelectArrayCIN currentCin;

private:
    int get_maxlen (const String &encoding);
    void reload_config (const ConfigPointer &config);
    void load_user_phrases();

    Property                    m_status_property;
    Property                    m_letter_property;

    ConfigPointer               m_config;

    KeyEventList                m_ench_key;
    KeyEventList                m_full_half_key;
    bool                        m_show_special;
    bool                        m_special_code_only;
    const bool                  m_use_phrases;

    Connection                  m_reload_signal_connection;
};

class ArrayInstance : public IMEngineInstanceBase
{
    Pointer <ArrayFactory>    m_factory;

    CommonLookupTable           m_lookup_table;
    std::vector<WideString>     m_lookup_table_labels;
    WideString                  m_preedit_string;
    WideString                  m_aux_string;
    const bool                  &m_show_special;
    const bool                  &m_special_code_only;
    const bool                  &m_use_phrases;

    size_t                      m_max_preedit_len;

    int                         commit_press_count;

    bool                        m_forward;
    bool                        m_full_width_letter;

public:
    ArrayInstance (ArrayFactory *factory,
                     const String& encoding,
                     int id = -1);
    virtual ~ArrayInstance ();

    virtual bool process_key_event (const KeyEvent& key);
    virtual void move_preedit_caret (unsigned int pos);
    virtual void select_candidate (unsigned int item);
    virtual void update_lookup_table_page_size (unsigned int page_size);
    virtual void lookup_table_page_up ();
    virtual void lookup_table_page_down ();
    virtual void reset ();
    virtual void focus_in ();
    virtual void focus_out ();
    virtual void trigger_property (const String &property);

private:
    int create_lookup_table (int);
    int create_phrase_lookup_table();
    void process_preedit_string ();
    void process_symbol_preedit_string ();
    void pre_update_preedit_string(const WideString&);

    void create_lookup_table_labels(int page_size); 

    void space_key_press();
    void phrase_key_press();

    void send_commit_string(const WideString&, const WideString&);

    void initialize_properties ();
    void refresh_status_property ();
    void refresh_letter_property ();

    bool show_special_code(const WideString&, const WideString&);
    bool show_pre_special_code(const WideString&);
    bool check_special_code_only(const WideString&, const WideString&);

    bool match_key_event (const KeyEventList&, const KeyEvent&) const;
};

#endif
/*
vi:ts=4:nowrap:ai:expandtab
*/

