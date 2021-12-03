/***************************************************************************
 *          tapParser.h
 *
 *  Copyright  2006 - 2013 Florent Berthaut, 2019 Jean-Emmanuel Doucet & Aurélien Roux
 *  florentberthaut@no-log.org jean-emmanuel.doucet@groolot.net orl@ammd.net
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef TAPPARSER_H
#define TAPPARSER_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include "kit.h"
#include "instrument.h"
#include "variation.h"
#include "setup.h"

class tapeutape;

class tapParser
{
    public:
        tapParser(tapeutape*);
        ~tapParser();
        int createTree(char*);
        int parseTree();
        int recParseTree(xmlNode*);

        int parse005(xmlNodePtr);
        int parseCurrent(xmlNodePtr);

        int parseOptions(xmlNodePtr);
        int parseCreation(xmlNodePtr);
        int parseSetup(xmlNodePtr);
        int parseKit(xmlNodePtr,setup*);
        int parseInstrument(xmlNodePtr,kit*);
        int parseVariation(xmlNodePtr,instrument*);

        int saveToFile(char*);

    private:

        char* fileName;
        xmlDocPtr doc;
        xmlNode *root_element;
        tapeutape *tap;
        bool creationMode;
        bool optionsMode;
        kit* currentKit;
        instrument* currentInstru;
        variation* currentVar;
        setup* currentSetup;
};


#endif
