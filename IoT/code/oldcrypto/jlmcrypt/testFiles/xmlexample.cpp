/*
<?xml version=\"1.0\"  standalone='no' >
    <!-- Our to do list data -->
    <ToDo>
         <!-- Do I need a secure PDA? -->
         <Item priority="1" distance='close'> Go to the <bold>Toy store!</bold></Item>
         <Item priority="2" distance='none'> Do bills   </Item>"
         <Item priority="2" distance='far &amp; back'> Look for Evil Dinosaurs! </Item>
    </ToDo> 
*/


TiXmlDocument doc( "demotest.xml" );
doc.Parse( demoStart );
if(doc.Error()) {
    printf( "Error in %s: %s\n", doc.Value(), doc.ErrorDesc() );
    }
doc.SaveFile();

TiXmlDocument doc( "demotest.xml" );
bool loadOkay = doc.LoadFile();
doc.Print( stdout );
TiXmlNode* node = 0;
TiXmlElement* todoElement = 0;
TiXmlElement* itemElement = 0;

// --------------------------------------------------------
// An example of changing existing attributes, and removing
// an element from the document.
// --------------------------------------------------------

// Get the "ToDo" element.
// It is a child of the document, and can be selected by name.
node = doc.FirstChild( "ToDo" );
todoElement = node->ToElement();

// Going to the toy store is now our second priority...
// So set the "priority" attribute of the first item in the list.
node = todoElement->FirstChildElement();        // This skips the "PDA" comment.
itemElement = node->ToElement();
itemElement->SetAttribute( "priority", 2 );

// Change the distance to "doing bills" from
// "none" to "here". It's the next sibling element.
itemElement = itemElement->NextSiblingElement();
itemElement->SetAttribute( "distance", "here" );

// Remove the "Look for Evil Dinosaurs!" item.
// It is 1 more sibling away. We ask the parent to remove
// a particular child.
itemElement = itemElement->NextSiblingElement();
todoElement->RemoveChild( itemElement );

itemElement = 0;

// --------------------------------------------------------
// What follows is an example of created elements and text
// nodes and adding them to the document.
// --------------------------------------------------------

// Add some meetings.
TiXmlElement item( "Item" );
item.SetAttribute( "priority", "1" );
item.SetAttribute( "distance", "far" );

TiXmlText text( "Talk to:" );

TiXmlElement meeting1( "Meeting" );
meeting1.SetAttribute( "where", "School" );

TiXmlElement meeting2( "Meeting" );
meeting2.SetAttribute( "where", "Lunch" );

TiXmlElement attendee1( "Attendee" );
attendee1.SetAttribute( "name", "Marple" );
attendee1.SetAttribute( "position", "teacher" );

TiXmlElement attendee2( "Attendee" );
attendee2.SetAttribute( "name", "Voel" );
attendee2.SetAttribute( "position", "counselor" );

// Assemble the nodes we've created:
meeting1.InsertEndChild( attendee1 );
meeting1.InsertEndChild( attendee2 );

item.InsertEndChild( text );
item.InsertEndChild( meeting1 );
item.InsertEndChild( meeting2 );

// And add the node to the existing list after the first child.
node = todoElement->FirstChild( "Item" );
itemElement = node->ToElement();

todoElement->InsertAfterChild( itemElement, item );

doc.Print( stdout );


TiXmlDocument doc;
TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
TiXmlElement * element = new TiXmlElement( "Hello" );
TiXmlText * text = new TiXmlText( "World" );
element->LinkEndChild( text );
doc.LinkEndChild( decl );
doc.LinkEndChild( element );
doc.SaveFile( "madeByHand.xml" );


