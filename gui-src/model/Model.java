package model;

import java.io.FileWriter;
import java.io.IOException;

import org.json.simple.JSONArray;

import view.View;

/**
 * The Model class in the MVC pattern.
 */
public class Model {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Singleton Construction
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Singleton instance of Model class
     */
    private static final Model instance = new Model();

    /**
     * Return an instance of the Model class
     *
     * @return An instance of the Model class
     */
    public static Model getInstance() { return instance; }

    /**
     * Private default constructor for singleton
     */
    private Model() {}

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Getter Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return an instance of the config model
     *
     * @return An instance of the config model
     */
    public static ConfigModel getConfig() {
        return ConfigModel.getInstance();
    }

    /**
     * Return an instance of the spaces model
     *
     * @return An instance of the spaces model
     */
    public static SpacesModel getSpaces() {
        return SpacesModel.getInstance();
    }

    /**
     * Return an instance of the people model
     *
     * @return An instance of the people model
     */
    public static PeopleModel getPeople() {
        return PeopleModel.getInstance();
    }

    /**
     * Return an instance of the events model
     *
     * @return An instance of the events model
     */
    public static EventsModel getEvents() {
        return EventsModel.getInstance();
    }

    /**
     * Return an instance of the sensors model
     *
     * @return An instance of the sensors model
     */
    public static SensorsModel getSensors() {
        return SensorsModel.getInstance();
    }

    /**
     * Return an instance of the metapeople model
     *
     * @return An instance of the metapeople model
     */
    public static MetaPeopleModel getMetaPeople() {
        return MetaPeopleModel.getInstance();
    }

    /**
     * Return an instance of the metaevents model
     *
     * @return An instance of the metaevents model
     */
    public static MetaEventsModel getMetaEvents() {
        return MetaEventsModel.getInstance();
    }
    
    /**
     * Return an instance of the metasensors model
     *
     * @return An instance of the metasensors model
     */
    public static MetaSensorsModel getMetaSensors() {
        return MetaSensorsModel.getInstance();
    }

    /**
     * Return an instance of the spaces backdrop model
     *
     * @return An instance of the spaces backdrop model
     */
    public static BackdropModel getBackdrop() {
        return BackdropModel.getInstance();
    }

    /**
     * Return an instance of the time profiles model
     *
     * @return An instance of the time profiles model
     */
    public static TimeProfilesModel getTimeProfiles() {
        return TimeProfilesModel.getInstance();
    }
    
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////
    
    /**
     * Writes the data array into the file 
     * 
     * @param fname The name of the file to write
     * @param data  The data to write
     * @param cls   The class name
     */
    public void writeJSON(String fname, JSONArray data, String cls) {
        try {
            FileWriter f = new FileWriter(fname);
            f.write(data.toJSONString());
            f.close();
            View.log("Saved " + cls + " file: " + fname);
        } catch (IOException e) {
            View.log("Error: Cannot write " + cls + " to file: " + fname);
        }
    }

}
