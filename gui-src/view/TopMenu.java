package view;

import controller.Controller;
import javafx.scene.control.Menu;
import javafx.scene.control.MenuBar;
import javafx.scene.control.MenuItem;

/**
 * The top menu bar used in the SmartSPEC GUI
 */
public class TopMenu extends MenuBar {
    
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Constructs a MenuBar with the appropriate options for the GUI
     */
    public TopMenu() {

        Controller ctrl = Controller.getInstance();

        // Config menu
        Menu     configMenu      = new Menu    ("Config");
        MenuItem addOrEditConfig = new MenuItem("Add/Edit Config");
        MenuItem loadConfigFile  = new MenuItem("Load Config File");
        addOrEditConfig.setOnAction(e -> ctrl.handleAddNewConfig());
        loadConfigFile .setOnAction(e -> ctrl.handleLoadConfigFile());
        configMenu.getItems().addAll(
                addOrEditConfig, 
                loadConfigFile);

        // Spaces menu
        Menu     spacesMenu     = new Menu    ("Spaces");
        MenuItem saveSpacesFile = new MenuItem("Save Spaces File");
        MenuItem loadSpacesFile = new MenuItem("Load Spaces File");
        MenuItem loadBackdrop   = new MenuItem("Load Backdrop");
        saveSpacesFile.setOnAction(e -> ctrl.handleSaveSpacesFile());
        loadSpacesFile.setOnAction(e -> ctrl.handleLoadSpacesFile());
        loadBackdrop  .setOnAction(e -> ctrl.handleLoadBackdrop());
        spacesMenu.getItems().addAll(
                saveSpacesFile, 
                loadSpacesFile, 
                loadBackdrop);

        // Events menu
        Menu     eventsMenu     = new Menu    ("Events");
        MenuItem saveEventsFile = new MenuItem("Save Events File");
        MenuItem loadEventsFile = new MenuItem("Load Events File");
        saveEventsFile.setOnAction(e -> ctrl.handleSaveEventsFile());
        loadEventsFile.setOnAction(e -> ctrl.handleLoadEventsFile());
        eventsMenu.getItems().addAll(
                saveEventsFile, 
                loadEventsFile);

        // People menu
        Menu     peopleMenu     = new Menu    ("People");
        MenuItem addNewPerson   = new MenuItem("Declare People");
        MenuItem savePeopleFile = new MenuItem("Save People File");
        addNewPerson  .setOnAction(e -> ctrl.handleAddNewPerson());
        savePeopleFile.setOnAction(e -> ctrl.handleSavePeopleFile());
        peopleMenu.getItems().addAll(
                addNewPerson, 
                savePeopleFile);
        
        // Sensors meu
        Menu     sensorsMenu     = new Menu    ("Sensors");
        MenuItem saveSensorsFile = new MenuItem("Save Sensors File");
        MenuItem loadSensorsFile = new MenuItem("Load Sensors File");
        saveSensorsFile.setOnAction(e -> ctrl.handleSaveSensorsFile());
        loadSensorsFile.setOnAction(e -> ctrl.handleLoadSensorsFile());
        sensorsMenu.getItems().addAll(
                saveSensorsFile, 
                loadSensorsFile);

        // MetaEvents menu
        Menu     metaeventsMenu     = new Menu    ("MetaEvents");
        MenuItem declareMetaEvents  = new MenuItem("Declare MetaEvents");
        MenuItem editMetaEvent      = new MenuItem("Edit MetaEvent...");
        MenuItem saveMetaEventsFile = new MenuItem("Save MetaEvents File");
        MenuItem loadMetaEventsFile = new MenuItem("Load MetaEvents File");
        declareMetaEvents .setOnAction(e -> ctrl.handleDeclareMetaEvents());
        editMetaEvent     .setOnAction(e -> ctrl.handleEditMetaEvent());
        saveMetaEventsFile.setOnAction(e -> ctrl.handleSaveMetaEventsFile());
        loadMetaEventsFile.setOnAction(e -> ctrl.handleLoadMetaEventsFile());
        metaeventsMenu.getItems().addAll(
                declareMetaEvents, 
                editMetaEvent, 
                saveMetaEventsFile, 
                loadMetaEventsFile);

        // MetaPeople menu
        Menu     metapeopleMenu     = new Menu    ("MetaPeople");
        MenuItem declareMetaPeople  = new MenuItem("Declare MetaPeople");
        MenuItem editMetaPerson     = new MenuItem("Edit MetaPerson...");
        MenuItem saveMetaPeopleFile = new MenuItem("Save MetaPeople File");
        MenuItem loadMetaPeopleFile = new MenuItem("Load MetaPeople File");
        declareMetaPeople .setOnAction(e -> ctrl.handleDeclareMetaPeople());
        editMetaPerson    .setOnAction(e -> ctrl.handleEditMetaPerson());
        saveMetaPeopleFile.setOnAction(e -> ctrl.handleSaveMetaPeopleFile());
        loadMetaPeopleFile.setOnAction(e -> ctrl.handleLoadMetaPeopleFile());
        metapeopleMenu.getItems().addAll(
                declareMetaPeople, 
                editMetaPerson, 
                saveMetaPeopleFile, 
                loadMetaPeopleFile);
    
        // MetaSensors menu
        Menu     metasensorsMenu     = new Menu    ("MetaSensors");
        MenuItem declareMetaSensors  = new MenuItem("Declare MetaSensors");
        MenuItem saveMetaSensorsFile = new MenuItem("Save MetaSensors File");
        MenuItem loadMetaSensorsFile = new MenuItem("Load MetaSensors File");
        declareMetaSensors .setOnAction(e -> ctrl.handleDeclareMetaSensors());
        saveMetaSensorsFile.setOnAction(e -> ctrl.handleSaveMetaSensorsFile());
        loadMetaSensorsFile.setOnAction(e -> ctrl.handleLoadMetaSensorsFile());
        metasensorsMenu.getItems().addAll(
                declareMetaSensors,
                saveMetaSensorsFile,
                loadMetaSensorsFile);
        
        // TimeProfiles menu
        Menu     timeProfileMenu     = new Menu    ("Time Profiles");
        MenuItem addNewTimeProfile   = new MenuItem("Add New Time Profile");
        MenuItem saveTimeProfileFile = new MenuItem("Save Time Profile File");
        MenuItem loadTimeProfileFile = new MenuItem("Load Time Profile File");
        addNewTimeProfile  .setOnAction(e -> ctrl.handleAddNewTimeProfile());
        saveTimeProfileFile.setOnAction(e -> ctrl.handleSaveTimeProfileFile());
        loadTimeProfileFile.setOnAction(e -> ctrl.handleLoadTimeProfileFile());
        timeProfileMenu.getItems().addAll(
                addNewTimeProfile, 
                saveTimeProfileFile, 
                loadTimeProfileFile);

        // Add all menus to GUI
        this.getMenus().addAll(
                configMenu, 
                spacesMenu, 
                eventsMenu, 
                peopleMenu, 
                sensorsMenu,
                metaeventsMenu, 
                metapeopleMenu,
                metasensorsMenu,
                timeProfileMenu);
    }

}
