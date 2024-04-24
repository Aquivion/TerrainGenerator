#pragma once

#include <unordered_map>

#include "block.h"
#include "button.h"
#include "label.h"

/**
 * \brief The Panel class which is a subclass of the Block class.
 *
 * A Panel object can store buttons, labels and other panels in maps to get
 * access to them from the outside. The text functio of the base class is
 * overloaded, so that the text appears in the top middle of the panel.
 */
class Panel: public Block{
    public:
		/**
		* \brief Constructs a panel at the given area.
		*
		* \param[in] posX X-Position of the top left corner of the panel block.
		*
		* \param[in] posY Y-Position of the top left corner of the panel block.
		*
		* \param[in] width Width of the panel block.
		*
		* \param[in] height Height of the panel block.
		*/
        Panel(int posX, int posY, unsigned width, unsigned height);

        /**
		* \brief Destructs alls elements stored in the panel and the panel itself.
		*/
        ~Panel();

		/**
		* \brief Setting text in the top middle of the panel.
		*
		* \param[in] t text to be displayed.
		*
		* \param[in] font Font for the text.
		*/
        void text(wstring t, Font& font);

		/**
		* \brief Draws the panel and all it's sub elements.
		*/
        void update();

		/**
		* \brief Adds another panel to the panel map
		*
		* \param[in] p Panel to be added.
		*
		* \param[in] index Index string to that indicates the panel.
		*/
        void addPanel(Panel *p, string index);

		/**
		* \brief Adds a button to the button map
		*
		* \param[in] b Button to be added.
		*
		* \param[in] index Index string to that indicates the button.
		*/
        void addButton(Button* b, string index);

		/**
		* \brief Adds a label to the button map
		*
		* \param[in] l Label to be added.
		*
		* \param[in] index Index string to that indicates the label.
		*/
        void addLabel(Label* l, string index);

		/**
		* \brief Returns the panel with the given index
		*
		* \param[in] index Given index.
		*/
        Panel* getPanelAt(string index)const;

		/**
		* \brief Returns the button with the given index
		*
		* \param[in] index Given index.
		*/
        Button* getButtonAt(string index)const;

		/**
		* \brief Returns the label with the given index
		*
		* \param[in] index Given index.
		*/
        Label* getLabelAt(string index)const;

    private:
		/**
		* \brief Unordered map for all sub panels.
		*/
        unordered_map<string, Panel*> m_panels;

		/**
		* \brief Unordered map for all sub buttons.
		*/
        unordered_map<string, Button*> m_buttons;

		/**
		* \brief Unordered map for all sub labels.
		*/
        unordered_map<string, Label*> m_labels;
};
