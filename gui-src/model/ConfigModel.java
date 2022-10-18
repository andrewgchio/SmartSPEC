package model;

import java.io.File;
import java.io.IOException;
import java.nio.file.Paths;
import java.time.LocalDate;
import java.time.format.DateTimeFormatter;

import org.ini4j.Ini;
import org.ini4j.InvalidFileFormatException;

import modelutils.ErrorStringBuilder;
import modelutils.MyFileWriter;
import view.View;

public class ConfigModel {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Singleton Construction
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Singleton instance of ConfigModel class
     */
    private static final ConfigModel instance = new ConfigModel();

    /**
     * Return an instance of the ConfigModel class
     * 
     * @return An instance of the ConfigModel class
     */
    public static ConfigModel getInstance() { return instance; }

    /**
     * Private default constructor for singleton
     */
    private ConfigModel() {}

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////////

    // People Generation
    private Integer peopleNumber = 0;
    private String  peopleGen    = null;

    // Events Generation
    private Integer eventsNumber = 0;
    private String  eventsGen    = null;

    // Synthetic Data Generation
    private LocalDate start = null;
    private LocalDate end   = null;

    // File paths
    private String metapeoplePath  = null;
    private String metaeventsPath  = null;
    private String metasensorsPath = null;
    private String peoplePath      = null;
    private String eventsPath      = null;
    private String spacesPath      = null;
    private String sensorsPath     = null;
    private String outputDirPath   = null;
    private String baseDir         = null; // generated files path
    private String pathCachePath   = null;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Getter Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return the number of people to generate
     * 
     * @return The number of people to generate
     */
    public Integer getPeopleNumber() {
        return peopleNumber;
    }

    /**
     * Return the generation mechanism for people
     * 
     * @return The generation mechanism for people
     */
    public String getPeopleGen() {
        return peopleGen;
    }

    /**
     * Return the number of events to generate
     * 
     * @return The number of events to generate
     */
    public Integer getEventsNumber() {
        return eventsNumber;
    }

    /**
     * Return the generation mechanism for events
     * 
     * @return The generation mechanism for events
     */
    public String getEventsGen() {
        return eventsGen;
    }

    /**
     * Return the start date of the simulation
     * 
     * @return The start date of the simulation
     */
    public LocalDate getStart() {
        return start;
    }

    /**
     * Return the end date of the simulation
     * 
     * @return The end date of the simulation
     */
    public LocalDate getEnd() {
        return end;
    }

    /**
     * Return the path to the metapeople file
     * 
     * @return The path to the metapeople file
     */
    public String getMetaPeoplePath() {
        return metapeoplePath;
    }

    /**
     * Return the path to the metaevents file
     * 
     * @return The path to the metaevents file
     */
    public String getMetaEventsPath() {
        return metaeventsPath;
    }

    /**
     * Return the path to the metasensors file
     * 
     * @return The path to the metasensors file
     */
    public String getMetaSensorsPath() {
        return metasensorsPath;
    }

    /**
     * Return the path to the people file
     * 
     * @return The path to the people file
     */
    public String getPeoplePath() {
        return peoplePath;
    }

    /**
     * Return the path to the events file
     * 
     * @return The path to the events file
     */
    public String getEventsPath() {
        return eventsPath;
    }

    /**
     * Return the path to the spaces file
     * 
     * @return The path to the spaces file
     */
    public String getSpacesPath() {
        return spacesPath;
    }

    /**
     * Return the path to the sensors file
     * 
     * @return The path to the sensors file
     */
    public String getSensorsPath() {
        return sensorsPath;
    }

    /**
     * Return the path to the output directory
     * 
     * @return The path to the output directory
     */
    public String getOutputDirPath() {
        return outputDirPath;
    }

    /**
     * Return the path to the base directory
     * 
     * @return The path to the base directory
     */
    public String getBaseDir() {
        return baseDir;
    }

    /**
     * Return the path to the path cache file
     * 
     * @return The path to the path cache file
     */
    public String getPathCachePath() {
        return pathCachePath;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Setter Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Set the number of people to generate
     * 
     * @param n The number of people to generate
     */
    public void setPeopleNumber(Integer n) {
        if (n < 0)
            throw new IllegalArgumentException("Negative people number");

        peopleNumber = n;
    }

    /**
     * Set the generation mechanism for people
     * 
     * @param gen The generation mechanism for people
     */
    public void setPeopleGen(String gen) {
        if (!gen.equals("all") && !gen.equals("diff") && !gen.equals("none"))
            throw new IllegalArgumentException("Invalid people generation");

        peopleGen = gen;
    }

    /**
     * Set the number of events to generate
     * 
     * @param n The number of events to generate
     */
    public void setEventsNumber(Integer n) {
        if (n < 0)
            throw new IllegalArgumentException("Negative events number");

        eventsNumber = n;
    }

    /**
     * Set the generation mechanism for events
     * 
     * @param gen The generation mechanism for events
     */
    public void setEventsGen(String gen) {
        if (!gen.equals("all") && !gen.equals("diff") && !gen.equals("none"))
            throw new IllegalArgumentException("Invalid events generation");

        eventsGen = gen;
    }

    /**
     * Set the start date of the simulation
     * 
     * @param date The start date of the simulation
     */
    public void setStart(LocalDate date) {
        if (end != null && end.isBefore(date))
            throw new IllegalArgumentException("Start cannot be after end");

        start = date;
    }

    /**
     * Set the end date of the simulation
     * 
     * @param date The end date of the simulation
     */
    public void setEnd(LocalDate date) {
        if (start != null && start.isAfter(date))
            throw new IllegalArgumentException("End cannot be before start");

        end = date;
    }

    /**
     * Set the path to the metapeople file
     * 
     * @param path The metapeople file path
     */
    public void setMetaPeoplePath(String path) {
        metapeoplePath = path;
    }

    /**
     * Set the path to the metaevents file
     * 
     * @param path The metaevents file path
     */
    public void setMetaEventsPath(String path) {
        metaeventsPath = path;
    }

    /**
     * Set the path to the metasensors file
     * 
     * @param path The metasensors file path
     */
    public void setMetaSensorsPath(String path) {
        metasensorsPath = path;
    }

    /**
     * Set the path to the people file
     * 
     * @param path The people file path
     */
    public void setPeoplePath(String path) {
        peoplePath = path;
    }

    /**
     * Set the path to the events file
     * 
     * @param path The events file path
     */
    public void setEventsPath(String path) {
        eventsPath = path;
    }

    /**
     * Set the path to the spaces file
     * 
     * @param path The spaces file path
     */
    public void setSpacesPath(String path) {
        spacesPath = path;
    }

    /**
     * Set the path to the sensors file
     * 
     * @param path The sensors file path
     */
    public void setSensorsPath(String path) {
        sensorsPath = path;
    }

    /**
     * Set the path to the output directory
     * 
     * @param path The output directory path
     */
    public void setOutputDirPath(String path) {
        outputDirPath = path;
    }

    /**
     * Set the path to the base directory
     * 
     * @param path The base directory path
     */
    public void setBaseDir(String path) {
        baseDir = path;
    }

    /**
     * Set the path to the path cache file
     * 
     * @param path The path cache file path
     */
    public void setPathsCache(String path) {
        pathCachePath = path;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Other Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return true if the config model is valid
     * 
     * @param print Set to true to print errors
     * @return True if the config model is valid
     */
    public boolean checkValid(boolean print) {
        // Check each entry
        ErrorStringBuilder err = new ErrorStringBuilder();
        if (peopleNumber == null)
            err.append("Invalid people number");
        if (peopleGen == null)
            err.append("Invalid people generation");
        if (eventsNumber == null)
            err.append("Invalid events number");
        if (eventsGen == null)
            err.append("Invalid events generation");
        if (start == null)
            err.append("Invalid start date");
        if (end == null)
            err.append("Invalid end date");
        if (metapeoplePath == null)
            err.append("Invalid metapeople path");
        if (metaeventsPath == null)
            err.append("Invalid metaevents path");
        if (metasensorsPath == null)
            err.append("Invalid metasensors path");
        if (peoplePath == null)
            err.append("Invalid people path");
        if (eventsPath == null)
            err.append("Invalid events path");
        if (spacesPath == null)
            err.append("Invalid spaces path");
        if (sensorsPath == null)
            err.append("Invalid sensors path");
        if (outputDirPath == null)
            err.append("Invalid output directory path");
        if (baseDir == null)
            err.append("Invalid base directory path");
        if (pathCachePath == null)
            err.append("Invalid path cache path");

        if (print && !err.isEmpty())
            View.log(err.toString());

        // if empty, no errors were found
        return err.isEmpty();
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Load the provided config file
     * 
     * @param f The file to load
     */
    public void loadFile(File f) {
        if (f == null) {
            View.log("Error: No config file selected");
            return;
        }

        try {
            Ini ini = new Ini(f);

            Integer peopleNumber = ini.get("people", "number", Integer.class);
            setPeopleNumber(peopleNumber);

            String peopleGen = ini.get("people", "generation", String.class);
            setPeopleGen(peopleGen);

            Integer eventsNumber = ini.get("events", "number", Integer.class);
            setEventsNumber(eventsNumber);

            String eventsGen = ini.get("events", "generation", String.class);
            setEventsGen(eventsGen);

            LocalDate start = LocalDate.parse(
                    ini.get("synthetic-data-generator", "start", String.class),
                    DateTimeFormatter.ISO_LOCAL_DATE);
            setStart(start);

            LocalDate end = LocalDate.parse(
                    ini.get("synthetic-data-generator", "end", String.class),
                    DateTimeFormatter.ISO_LOCAL_DATE);
            setEnd(end);

            String mpPath = ini.get("filepaths", "metapeople", String.class);
            setMetaPeoplePath(mpPath);

            String mePath = ini.get("filepaths", "metaevents", String.class);
            setMetaEventsPath(mePath);

            String msPath = ini.get("filepaths", "metasensors", String.class);
            setMetaSensorsPath(msPath);

            String pPath = ini.get("filepaths", "people", String.class);
            setPeoplePath(pPath);

            String ePath = ini.get("filepaths", "events", String.class);
            setEventsPath(ePath);

            String cPath = ini.get("filepaths", "spaces", String.class);
            setSpacesPath(cPath);

            String sPath = ini.get("filepaths", "sensors", String.class);
            setSensorsPath(sPath);

            String out = ini.get("filepaths", "output", String.class);
            setOutputDirPath(out);

            String base = ini.get("filepaths", "generated-files", String.class);
            setBaseDir(base);

            String pathCache = ini.get("filepaths", "path-cache", String.class);
            setPathsCache(pathCache);

            View.log("Loaded config file: " + f.getAbsolutePath());
        } 
        catch (IllegalArgumentException e) {
            View.log("Error: " + e.getMessage());
        } 
        catch (InvalidFileFormatException e) {
            View.log("Error: Invalid file format: " + f.getAbsolutePath());
        } 
        catch (IOException e) {
            View.log("Error: " + e.getMessage());
        }
    }

    /**
     * Write the config model to file
     */
    public void writeToFile() {
        if (!checkValid(true))
            return;

        try {
            String fname = Paths.get(baseDir, "config.txt").toString();

            MyFileWriter f = new MyFileWriter(fname);

            f.writeln("[people]");
            f.writeln("number     = " + eventsNumber);
            f.writeln("generation = " + eventsGen);
            f.writeln();

            f.writeln("[events]");
            f.writeln("number     = " + peopleNumber);
            f.writeln("generation = " + peopleGen);
            f.writeln();

            f.writeln("[synthetic-data-generator]");
            f.writeln("start = " + start);
            f.writeln("end   = " + end);
            f.writeln();

            f.writeln("[filepaths]");
            f.writeln("metapeople      = " + metapeoplePath);
            f.writeln("metaevents      = " + metaeventsPath);
            f.writeln("metasensors     = " + metasensorsPath);
            f.writeln("people          = " + peoplePath);
            f.writeln("events          = " + eventsPath);
            f.writeln("spaces          = " + spacesPath);
            f.writeln("sensors         = " + sensorsPath);
            f.writeln("output          = " + outputDirPath);
            f.writeln("generated-files = " + baseDir);
            f.writeln("path-cache      = " + pathCachePath);

            f.close();

            View.log("Saved config file: " + fname);
            
            // Also create an output directory
            new File(outputDirPath).mkdirs();
        } 
        catch (IOException e) {
            View.log("Error: Cannot write config to file");
        }

    }

}
