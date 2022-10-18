package form;

import java.io.File;
import java.nio.file.FileSystems;
import java.nio.file.Paths;

import javafx.event.ActionEvent;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.layout.VBox;
import javafx.stage.DirectoryChooser;
import javafx.stage.Window;
import model.ConfigModel;
import model.Model;
import viewutils.DateField;
import viewutils.IntField;

public class ConfigForm extends Form {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////////

    protected IntField          peopleNumberEntry;
    protected ChoiceBox<String> peopleGenEntry;

    protected IntField          eventsNumberEntry;
    protected ChoiceBox<String> eventsGenEntry;

    protected DateField startEntry;
    protected DateField endEntry;

    protected VBox dirEntry;

    public static final int DIR_WIDTH = 400;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Construct a Config Form
     */
    public ConfigForm() {
        super();

        ConfigModel cm = Model.getConfig();

        // Options for people
        addEntry("People");

        peopleNumberEntry = new IntField();
        peopleNumberEntry.setValue(cm.getPeopleNumber());
        addSubEntry("Number:", peopleNumberEntry);

        peopleGenEntry = new ChoiceBox<>();
        peopleGenEntry.setValue(cm.getPeopleGen());
        peopleGenEntry.getItems().addAll("all", "none", "diff");
        addSubEntry("Generation Strategy", peopleGenEntry);

        // Options for events
        addEntry("Events");

        eventsNumberEntry = new IntField();
        eventsNumberEntry.setValue(cm.getEventsNumber());
        addSubEntry("Number:", eventsNumberEntry);

        eventsGenEntry = new ChoiceBox<>();
        eventsGenEntry.setValue(cm.getEventsGen());
        eventsGenEntry.getItems().addAll("all", "none", "diff");
        addSubEntry("Generation Strategy", eventsGenEntry);

        // Options for synthetic data generator
        addEntry("Synthetic Data Generator");

        startEntry = new DateField();
        startEntry.setValue(cm.getStart());
        addSubEntry("Start Date:", startEntry);

        endEntry = new DateField();
        endEntry.setValue(cm.getEnd());
        addSubEntry("End Date:", endEntry);

        // Options for filepaths
        addEntry("Paths to Files");

        dirEntry = makeDirectoryEntry(cm.getBaseDir());
        addSubEntry("Select parent data directory:", dirEntry);

        addSave(this::saveForm);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Form Construction Helpers
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Create a button to select the base directory
     * 
     * @param baseDir The default basedir to set
     * @return A button the select the base directory
     */
    private VBox makeDirectoryEntry(String baseDir) {
        // Selected directory
        Label selection = new Label("None Selected");
        selection.setWrapText(true);
        selection.setMaxWidth(DIR_WIDTH);
        if (baseDir != null)
            selection.setText(baseDir);

        // Button to select directory
        Button selectButton = new Button("Select Directory");
        selectButton.setOnAction(e -> {
            Window w        = selectButton.getParent().getScene().getWindow();
            File   selected = new DirectoryChooser().showDialog(w);
            if (selected != null)
                selection.setText(selected.getAbsolutePath());
        });

        return new VBox(selection, selectButton);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Save the input provided by the user into a config file
     * 
     * @param e
     */
    private void saveForm(ActionEvent e) {

        ConfigModel cm = Model.getConfig();

        // people
        cm.setPeopleNumber(peopleNumberEntry.getValue());
        cm.setPeopleGen(peopleGenEntry.getValue());

        // events
        cm.setEventsNumber(eventsNumberEntry.getValue());
        cm.setEventsGen(eventsGenEntry.getValue());

        // synthetic-data-generator
        cm.setStart(startEntry.getDateValue());
        cm.setEnd(endEntry.getDateValue());

        // filepaths
        String baseDir = ((Label) dirEntry.getChildren().get(0)).getText();
        if (baseDir.equals("None Selected"))
            throw new IllegalArgumentException("No directory selected");

        cm.setMetaPeoplePath(Paths.get(baseDir, "MetaPeople.json").toString());
        cm.setMetaEventsPath(Paths.get(baseDir, "MetaEvents.json").toString());
        cm.setMetaSensorsPath(Paths.get(baseDir, "MetaSensors.json").toString());
        cm.setPeoplePath(Paths.get(baseDir, "People.json").toString());
        cm.setEventsPath(Paths.get(baseDir, "Events.json").toString());
        cm.setSpacesPath(Paths.get(baseDir, "Spaces.json").toString());
        cm.setSensorsPath(Paths.get(baseDir, "Sensors.json").toString());
        cm.setOutputDirPath(Paths.get(baseDir, "output").toString() + 
                FileSystems.getDefault().getSeparator());
        cm.setBaseDir(Paths.get(baseDir).toString() + 
                FileSystems.getDefault().getSeparator());
        cm.setPathsCache(
                Paths.get(baseDir, "output", "path-cache.csv").toString());

        cm.writeToFile();
    }

}
