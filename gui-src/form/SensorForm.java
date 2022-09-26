package form;

import java.util.HashSet;
import java.util.Set;
import java.util.stream.Collectors;

import javafx.event.ActionEvent;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import model.MetaSensorsModel;
import model.MetaSensorsModel.MetaSensor;
import model.Model;
import model.SensorsModel;
import model.SensorsModel.Sensor;
import model.SpacesModel.Space;
import view.View;
import viewutils.DoubleField;
import viewutils.IDChoiceBox;
import viewutils.IDComboBox;

public class SensorForm extends Form {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////////

    protected IDComboBox  id;
    protected IDChoiceBox metaid;
    protected TextField   description;
    protected HBox        coordinates;
    protected VBox        coverage;
    
    private Set<Integer> coveredSpaces = new HashSet<>();

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////
    
    /**
     * Construct a Sensor Form based on a space
     * 
     * @param c The hosting space
     */
    public SensorForm(Space c) {
        super();
        
        // id
        id = makeIDEntry(c);
        addEntry("Sensor ID:", id);
        
        // metasensor-id
        metaid = makeMetaSensorEntry();
        addEntry("MetaSensor ID:", metaid);

        // description
        description = new TextField();
        addEntry("Description", description);
        
        // coordinates
        coordinates = makeCoordinatesEntry();
        setCoordinates(c.getX(), c.getY(), c.getZ(), true);
        addEntry("Coordinates:", coordinates);
        
        // coverage
        coverage = makeSpacesEntry();
        coveredSpaces.add(c.getID());
        updateSpacesLabel((Label) coverage.getChildren().get(0));
        addEntry("Coverage:", coverage);
        
        addSave(this::saveForm);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Form Construction Helpers
    ////////////////////////////////////////////////////////////////////////////
    
    /**
     * Create an ID entry box that modifies all other entries when selected
     * 
     * @param c The hosting space
     * @return An ID entry box that modifies all other entries when selected
     */
    private IDComboBox makeIDEntry(Space c) {
        SensorsModel sm = Model.getSensors();

        IDComboBox entry = new IDComboBox(sm.stream(), 
                x -> ((Sensor) x).getCoverage().contains(c.getID()));

        entry.setOnAction(e2 -> {
            Sensor s = sm.get(entry.getSelectedID());

            // If sensor exists, then set all other data entries
            if (s != null) {
                // metaid
                metaid.setSelectedID(s.getMetaID());

                // description
                description.setText(s.getDescription());

                // coordinates will be grayed out and cannot be modified
                setCoordinates(c.getX(), c.getY(), c.getZ(), true);
                
                // coverage
                coveredSpaces.clear();
                coveredSpaces.addAll(s.getCoverage());
                updateSpacesLabel((Label) coverage.getChildren().get(0));

            }

        });

        return entry;
    }
    
    /**
     * Create an ID entry box that further modifies other entries when selected
     * 
     * @return A metasensor ID ChoiceBox
     */
    private IDChoiceBox makeMetaSensorEntry() {
        MetaSensorsModel msm = Model.getMetaSensors();

        IDChoiceBox entry = new IDChoiceBox(msm.stream());

        entry.setOnAction(e -> {
            MetaSensor ms = msm.get(entry.getSelectedID());
            
            // description
            description.setText(ms.getDescription());

        });

        return entry;
    }
    
    /**
     * Return the HBox to input coordinates
     * 
     * @return The HBox to input coordinates
     */
    private HBox makeCoordinatesEntry() {
        return new HBox(
                new DoubleField(),  // x
                new DoubleField(),  // y
                new DoubleField()); // z
    }
    
    
    /**
     * Create the data entry for spaces
     * 
     * @return The data entry for spaces
     */
    private VBox makeSpacesEntry() {
        // selected items
        Label selected = new Label("[]");

        // choices
        IDChoiceBox choice = new IDChoiceBox(Model.getSpaces().stream());

        Button addButton = new Button("+");
        addButton.setOnAction(e -> {
            Integer cid = choice.getSelectedID();
            coveredSpaces.add(cid);
            updateSpacesLabel(selected);
        });

        Button removeButton = new Button("-");
        removeButton.setOnAction(e -> {
            Integer cid = choice.getSelectedID();
            coveredSpaces.remove(cid);
            updateSpacesLabel(selected);
        });

        HBox selectionEntry = new HBox(choice, addButton, removeButton);
        selectionEntry.setSpacing(SPACING);

        return new VBox(selected, selectionEntry);
    }
    
    /**
     * Update the given label based on the selected spaces
     * 
     * @param spacesLabel The label to update
     */
    private void updateSpacesLabel(Label spacesLabel) {
        spacesLabel.setText(
                coveredSpaces.stream()
                              .sorted()
                              .map(x -> x.toString())
                              .collect(Collectors.joining(",", "[", "]")));
    }
    
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Setters
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Set the coordinates in the Space Form
     * 
     * @param x The x-coordinate to set
     * @param y The y-coordinate to set
     * @param z The z-coordinate to set
     */
    public void setCoordinates(double x, double y, double z) {
        ((DoubleField) coordinates.getChildren().get(0)).setValue(x);
        ((DoubleField) coordinates.getChildren().get(1)).setValue(y);
        ((DoubleField) coordinates.getChildren().get(2)).setValue(z);
    }
    
    /**
     * Set the coordinates in the Space Form (without edits)
     * @param x The x-coordinate to set
     * @param y The y-coordinate to set
     * @param z The z-coordinate to set
     * @param edit Set to true to disable editing
     */
    public void setCoordinates(double x, double y, double z, boolean edit) {
        DoubleField fieldX = (DoubleField) coordinates.getChildren().get(0);
        DoubleField fieldY = (DoubleField) coordinates.getChildren().get(1);
        DoubleField fieldZ = (DoubleField) coordinates.getChildren().get(2);
        
        if (edit) {
            fieldX.setValue(x);
            fieldX.setEditable(false);
            fieldX.setStyle("-fx-control-inner-background: #D3D3D3;");

            fieldY.setValue(y);
            fieldY.setEditable(false);
            fieldY.setStyle("-fx-control-inner-background: #D3D3D3;");

            fieldZ.setValue(z);
            fieldZ.setEditable(false);
            fieldZ.setStyle("-fx-control-inner-background: #D3D3D3;");
        }

    }
    
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Save the input provided by the user into a sensor
     * 
     * @param ae
     */
    private void saveForm(ActionEvent e) {
        SensorsModel sm = Model.getSensors();
        Sensor       s  = sm.new Sensor();

        // id
        s.setID(id.getSelectedID());

        // metaid
        s.setMetaID(metaid.getSelectedID());

        // description
        s.setDescription(description.getText());
        
        // coordinates
        s.setCoordinates(
                ((DoubleField) coordinates.getChildren().get(0)).getValue(),
                ((DoubleField) coordinates.getChildren().get(1)).getValue(),
                ((DoubleField) coordinates.getChildren().get(2)).getValue());

        // coverage
        s.getCoverage().clear();
        for (Integer n : coveredSpaces)
            s.addCoverage(n);

        sm.add(s);

        View.log("Saved sensor " + s.getDisplayName());

    }

}
