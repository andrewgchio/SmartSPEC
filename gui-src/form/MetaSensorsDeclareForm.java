package form;

import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.scene.Node;
import javafx.scene.control.Button;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import model.MetaSensorsModel;
import model.MetaSensorsModel.MetaSensor;
import model.Model;
import view.View;
import viewutils.IntField;

public class MetaSensorsDeclareForm extends Form {

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
    public MetaSensorsDeclareForm() {
        super();

        // declarations
        declare = makeMetaSensorsDeclareEntry();
        addEntry("Declare: ", declare);

        addSave(this::saveForm);

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Form Constructors Helpers
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Create the node to declare metasensors
     * 
     * @return The node to declare metasensors
     */
    private VBox makeMetaSensorsDeclareEntry() {
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

        // remove
        Button remove = new Button("-");
        remove.setOnAction(e -> entries.getChildren().remove(entry));

        entry.getChildren().addAll(id, description, remove);
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
        MetaSensorsModel msm = Model.getMetaSensors();

        ScrollPane scroll = (ScrollPane) declare.getChildren().get(0);
        for (Node n : ((VBox) scroll.getContent()).getChildren()) {
            ObservableList<Node> entry = ((HBox) n).getChildren();

            // id
            Integer id = ((IntField) entry.get(0)).getValue();
            if (msm.containsID(id)) {
                View.log("Duplicate metasensor id " + id + " (ignored)");
                continue;
            }

            // description
            String description = ((TextField) entry.get(1)).getText();

            // Create metasensor object
            MetaSensor ms = msm.new MetaSensor();
            ms.setID(id);
            ms.setDescription(description);
            msm.add(ms);

            View.log("Declared metasensor " + ms.getDisplayName());
        }

    }

}
