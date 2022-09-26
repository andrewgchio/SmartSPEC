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
import view.View;
import viewutils.DoubleField;
import viewutils.IntField;

public class MetaPeopleDeclareForm extends Form {

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
     * Construct a MetaPeople Declaration Form
     */
    public MetaPeopleDeclareForm() {
        super();

        // declarations
        declare = makeMetaPersonDeclareEntry();
        addEntry("Declare: ", declare);

        addSave(this::saveForm);

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Form Constructors Helpers
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Create the node to declare metapeople
     * 
     * @return The node to declare metapeople
     */
    private VBox makeMetaPersonDeclareEntry() {
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
        HBox entry = new HBox();
        entry.setSpacing(SPACING);

        // id
        IntField id = new IntField();

        // description
        TextField description = new TextField();

        // probability
        DoubleField pr = new DoubleField();

        // remove
        Button remove = new Button("-");
        remove.setOnAction(e -> entries.getChildren().remove(entry));

        entry.getChildren().addAll(id, description, pr, remove);
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
        MetaPeopleModel mpm = Model.getMetaPeople();

        ScrollPane scroll = (ScrollPane) declare.getChildren().get(0);
        for (Node n : ((VBox) scroll.getContent()).getChildren()) {
            ObservableList<Node> entry = ((HBox) n).getChildren();

            // id
            Integer id = ((IntField) entry.get(0)).getValue();
            if (mpm.containsID(id)) {
                View.log("Duplicate metaperson id " + id + " (ignored)");
                continue;
            }

            // description
            String description = ((TextField) entry.get(1)).getText();

            // probability
            Double pr = ((DoubleField) entry.get(2)).getValue();

            // Create metaperson object
            MetaPerson mp = mpm.new MetaPerson();
            mp.setID(id);
            mp.setDescription(description);
            mp.setProbability(pr);
            mpm.add(mp);

            View.log("Declared metaperson " + mp.getDisplayName());
        }

    }

}
