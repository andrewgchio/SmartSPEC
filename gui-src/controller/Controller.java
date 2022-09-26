package controller;

import form.BackdropForm;
import form.ConfigForm;
import form.MetaEventForm;
import form.MetaEventsDeclareForm;
import form.MetaPeopleDeclareForm;
import form.MetaPersonForm;
import form.MetaSensorsDeclareForm;
import form.PersonDeclareForm;
import form.TimeProfileForm;
import model.Model;
import view.View;

/**
 * The Controller class in the MVC pattern.
 */
public class Controller {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Singleton Construction
    ////////////////////////////////////////////////////////////////////////////

    /**
     *  Singleton instance of Controller class
     */
    private static final Controller instance = new Controller();

    /**
     * Return an instance of the Controller class
     *
     * @return An instance of the Controller class
     */
    public static Controller getInstance() { return instance; }

    /**
     * Private default constructor for singleton
     */
    private Controller() {}

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Config Menu Options
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Create a config form to fill
     */
    public void handleAddNewConfig() {
        new ConfigForm().show();
    }

    /**
     * Prompt for a config file and load its contents
     */
    public void handleLoadConfigFile() {
        Model.getConfig().loadFile(View.getInstance().promptFile());
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Spaces Menu Options
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Write current spaces into file
     */
    public void handleSaveSpacesFile() {
        Model.getSpaces().writeToFile();
    }

    /**
     * Prompt for a spaces file and load its contents
     */
    public void handleLoadSpacesFile() {
        Model.getSpaces().loadFile(View.getInstance().promptFile());
    }

    /**
     * Create a space backdrop form to fill
     */
    public void handleLoadBackdrop() {
        new BackdropForm().show();
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Events Menu Options
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Write current events into file
     */
    public void handleSaveEventsFile() {
        Model.getEvents().writeToFile();
    }

    /**
     * Prompt for an events file and load its contents
     */
    public void handleLoadEventsFile() {
        Model.getEvents().loadFile(View.getInstance().promptFile());
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // People Menu Options
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Create a person form to fill
     */
    public void handleAddNewPerson() {
        new PersonDeclareForm().show();
    }

    /**
     * Write current people into file
     */
    public void handleSavePeopleFile() {
        Model.getPeople().writeToFile();
    }
    
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Sensors Menu Options
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Write current sensors into file
     */
    public void handleSaveSensorsFile() {
        Model.getSensors().writeToFile();
    }

    /**
     * Prompt for an events file and load its contents
     */
    public void handleLoadSensorsFile() {
        Model.getSensors().loadFile(View.getInstance().promptFile());
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // MetaEvents Menu Options
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Declare a set of metaevents in the simulation
     */
    public void handleDeclareMetaEvents() {
        new MetaEventsDeclareForm().show();
    }

    /**
     * Edit a selected metaevent
     */
    public void handleEditMetaEvent() {
        new MetaEventForm().show();
    }

    /**
     * Write current metaevents into file
     */
    public void handleSaveMetaEventsFile() {
        Model.getMetaEvents().writeToFile();
    }

    /**
     * Prompt for a metaevents file and load its contents
     */
    public void handleLoadMetaEventsFile() {
        Model.getMetaEvents().loadFile(View.getInstance().promptFile());
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // MetaPeople Menu Options
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Declare a set of metapeople in the simulation
     */
    public void handleDeclareMetaPeople() {
        new MetaPeopleDeclareForm().show();
    }

    /**
     * Edit a selected metaperson
     */
    public void handleEditMetaPerson() {
        new MetaPersonForm().show();
    }

    /**
     * Write current metapeople into file
     */
    public void handleSaveMetaPeopleFile() {
        Model.getMetaPeople().writeToFile();
    }

    /**
     * Prompt for a metapeople file and load its contents
     */
    public void handleLoadMetaPeopleFile() {
        Model.getMetaPeople().loadFile(View.getInstance().promptFile());
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // MetaSensors Menu Options
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Declare a set of metasensor in the simulation
     */
    public void handleDeclareMetaSensors() {
        new MetaSensorsDeclareForm().show();
    }

    /**
     * Write current metasensors into file
     */
    public void handleSaveMetaSensorsFile() {
        Model.getMetaSensors().writeToFile();
    }

    /**
     * Prompt for a metasensors file and load its contents
     */
    public void handleLoadMetaSensorsFile() {
        Model.getMetaSensors().loadFile(View.getInstance().promptFile());
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Time Profile Menu Options
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Create a time profile form to fill
     */
    public void handleAddNewTimeProfile() {
        new TimeProfileForm().show();
    }

    /**
     * Write current time profiles into file
     */
    public void handleSaveTimeProfileFile() {
        Model.getTimeProfiles().writeToFile();
    }

    /**
     * Prompt for a time profile file and load its contents
     */
    public void handleLoadTimeProfileFile() {
        Model.getTimeProfiles().loadFile(View.getInstance().promptFile());
    }

}
