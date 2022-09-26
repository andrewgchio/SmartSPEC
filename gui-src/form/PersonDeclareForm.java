package form;

import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.scene.Node;
import javafx.scene.control.Button;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import model.MetaPeopleModel;
import model.MetaPeopleModel.MetaPerson;
import model.Model;
import model.PeopleModel;
import model.PeopleModel.Person;
import view.View;
import viewutils.IDChoiceBox;
import viewutils.IntField;
import viewutils.TPChoiceBox;

public class PersonDeclareForm extends Form {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////////

    protected VBox declare;

    public static final int DECLARE_WIDTH  = 300;
    public static final int DECLARE_HEIGHT = 200;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Construct a Person Declaration Form
     */
    public PersonDeclareForm() {
        super();

        // declarations
        declare = makePersonDeclareEntry();
        addEntry("Declare: ", declare);

        addSave(this::saveForm);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Form Construction Helpers
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Create the node to declare people
     * 
     * @return The node to declare people
     */
    private VBox makePersonDeclareEntry() {
        VBox declareEntries = new VBox();

        // ScrollPane of VBox entries
        ScrollPane scroll = new ScrollPane();
        scroll.setMinHeight(DECLARE_HEIGHT);
        scroll.setMinWidth(DECLARE_WIDTH);
        VBox entries = new VBox();
        entries.setSpacing(SPACING);
        scroll.setContent(entries);

        // Button to add new entries
        Button addButton = new Button("Add new");
        addButton.setOnAction(e -> {
            HBox entry = makeDeclareEntry(entries);
            entries.getChildren().add(entry);
        });

        // "Click addButton once
        addButton.fire();

        declareEntries.getChildren().addAll(scroll, addButton);
        return declareEntries;
    }

    /**
     * Create a declaration entry
     * 
     * @param entries The node to add this object
     * @return A declaration entry
     */
    private HBox makeDeclareEntry(VBox entries) {

        MetaPeopleModel mpm = Model.getMetaPeople();

        HBox entry = new HBox();
        entry.setSpacing(SPACING);

        // id
        IntField id = new IntField();

        // metaid
        IDChoiceBox metaid = new IDChoiceBox(mpm.stream());

        // description
        TextField description = new TextField();

        // time profile
        TPChoiceBox tp = new TPChoiceBox();

        // set time profile options based on metaid
        metaid.setOnAction(e -> {
            MetaPerson mp = mpm.get(metaid.getSelectedID());
            tp.setOptions(mp);
        });

        // remove
        Button remove = new Button("-");
        remove.setOnAction(e -> entries.getChildren().remove(entry));

        entry.getChildren().addAll(id, metaid, description, tp, remove);
        return entry;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Save the input provided by the user into metapeople
     * 
     * @param e
     */
    private void saveForm(ActionEvent e) {
        PeopleModel pm = Model.getPeople();

        ScrollPane scroll = (ScrollPane) declare.getChildren().get(0);
        for (Node n : ((VBox) scroll.getContent()).getChildren()) {
            ObservableList<Node> entry = ((HBox) n).getChildren();

            // id
            Integer id = ((IntField) entry.get(0)).getValue();
            if (pm.containsID(id)) {
                View.log("Duplicate person id " + id + " (ignored)");
                continue;
            }

            // metaid
            Integer metaid = ((IDChoiceBox) entry.get(1)).getSelectedID();

            // description
            String description = ((TextField) entry.get(2)).getText();

            // time profiel
            String tp = ((TPChoiceBox) entry.get(3)).getSelectedProfile();

            // Create person object
            Person p = pm.new Person();
            p.setID(id);
            p.setMetaID(metaid);
            p.setDescription(description);
            p.setProfile(tp);
            pm.add(p);

            View.log("Saved person " + p.getDisplayName());
        }

    }

}
