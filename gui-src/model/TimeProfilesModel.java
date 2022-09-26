package model;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.time.LocalDate;
import java.time.LocalTime;
import java.time.format.DateTimeFormatter;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.Map;
import java.util.Set;
import java.util.stream.Stream;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import modelutils.ErrorStringBuilder;
import view.View;

public class TimeProfilesModel 
implements Iterable<model.TimeProfilesModel.TimeProfile> {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Singleton Construction
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Singleton instance of the TimeProfilesModel class
     */
    private static final TimeProfilesModel instance = new TimeProfilesModel();

    /**
     * Return an instance of the TimeProfilesModel class
     * 
     * @return An instance of the TimeProfilesModel class
     */
    public static TimeProfilesModel getInstance() { return instance; }

    /**
     * Private default constructor for singleton
     */
    private TimeProfilesModel() {}

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////////

    /**
     * A map of the time profile name to associated time profile
     */
    private Map<String, TimeProfile> profiles = new HashMap<>();

    /**
     * A map of the time profile description to the time profile name
     */
    private Map<String, String> profilesByDescription = new HashMap<>();

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Time Profile Definition
    ////////////////////////////////////////////////////////////////////////////

    public class TimeProfile {

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Instance Variables
        ////////////////////////////////////////////////////////////////////////

        private String    name       = null;
        private LocalDate startDate  = null, endDate  = null;
        private LocalTime startTime  = null, endTime  = null;
        private LocalTime startStdev = null, endStdev = null;
        private LocalTime reqTime    = null, reqStdev = null;

        private Integer repeat = 1;
        private Period  period = null;

        // Only for period = week
        private Set<String> days = new LinkedHashSet<>();

        private enum Period { DAY, WEEK }

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Constants
        ////////////////////////////////////////////////////////////////////////

        /**
         * Output date format as string "yyyy-MM-dd"
         */
        public final static String D_FORMAT = "yyyy-MM-dd";

        /**
         * Formatter that follows the pattern "yyyy-MM-dd"
         */
        public final static DateTimeFormatter D_FORMATTER = 
                DateTimeFormatter.ofPattern(D_FORMAT);

        /**
         * Output time format as string "HH:mm"
         */
        public final static String T_FORMAT = "HH:mm";

        /**
         * Formatter that follows the pattern "HH:mm"
         */
        public final static DateTimeFormatter T_FORMATTER = 
                DateTimeFormatter.ofPattern(T_FORMAT);

        /**
         * Formatter the follows the ISO format for time
         */
        public final static DateTimeFormatter T_LAZY_FORMATTER = 
                DateTimeFormatter.ISO_LOCAL_TIME;

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Getter Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Return the name of the time profile
         * 
         * @return The name of the time profile
         */
        public String getName() {
            return name;
        }

        /**
         * Return the start date of the time profile
         * 
         * @return The start date of the time profile
         */
        public LocalDate getStartDate() {
            return startDate;
        }

        /**
         * Return the end date of the time profile
         * 
         * @return The end date of the time profile
         */
        public LocalDate getEndDate() {
            return endDate;
        }

        /**
         * Return the start time of the time profile
         * 
         * @return The start time of the time profile
         */
        public LocalTime getStartTime() {
            return startTime;
        }

        /**
         * Return the end time of the time profile
         * 
         * @return The end time of the time profile
         */
        public LocalTime getEndTime() {
            return endTime;
        }
        
        /**
         * Return the required time of the time profile
         * 
         * @return The required time of the time profile
         */
        public LocalTime getReqTime() {
            return reqTime;
        }

        /**
         * Return the start time stdev of the time profile
         * 
         * @return The start time stdev of the time profile
         */
        public LocalTime getStartStdev() {
            return startStdev;
        }

        /**
         * Return the end time stdev of the time profile
         * 
         * @return The end time stdev of the time profile
         */
        public LocalTime getEndStdev() {
            return endStdev;
        }
        
        /**
         * Return the req time stdev of the time profile
         * 
         * @return The req time stdev of the time profile
         */
        public LocalTime getReqStdev() {
            return reqStdev;
        }

        /**
         * Return the start date of the time profile as a string
         * 
         * @return The start date of the time profile as a string
         */
        public String getStartDateStr() {
            return startDate.format(D_FORMATTER);
        }

        /**
         * Return the end date of the time profile as a string
         * 
         * @return The end date of the time profile as a string
         */
        public String getEndDateStr() {
            return endDate.format(D_FORMATTER);
        }

        /**
         * Return the start time of the time profile as a string
         * 
         * @return The start time of the time profile as a string
         */
        public String getStartTimeStr() {
            return startTime.format(T_FORMATTER);
        }

        /**
         * Return the end time of the time profile as a string
         * 
         * @return The end time of the time profile as a string
         */
        public String getEndTimeStr() {
            return endTime.format(T_FORMATTER);
        }

        /**
         * Return the req time of the time profile as a string
         * 
         * @return The req time of the time profile as a string
         */
        public String getReqTimeStr() {
            return reqTime.format(T_FORMATTER);
        }

        /**
         * Return the start time stdev of the time profile as a string
         * 
         * @return The start time stdev of the time profile as a string
         */
        public String getStartStdevStr() {
            return startStdev.format(T_FORMATTER);
        }

        /**
         * Return the end time stdev of the time profile as a string
         * 
         * @return The end time stdev of the time profile as a string
         */
        public String getEndStdevStr() {
            return endStdev.format(T_FORMATTER);
        }

        /**
         * Return the req time stdev of the time profile as a string
         * 
         * @return The req time stdev of the time profile as a string
         */
        public String getReqStdevStr() {
            return reqStdev.format(T_FORMATTER);
        }

        /**
         * Return the repeat frequency of the time profile
         * 
         * @return The repeat frequency of the time profile
         */
        public Integer getRepeat() {
            return repeat;
        }

        /**
         * Return the repeat period of the time profile
         * 
         * @return The repeat period of the time profile
         */
        public String getPeriod() {
            switch (period) {
            case DAY:  return "day";
            case WEEK: return "week";
            default:   return null;
            }
        }

        /**
         * Return the set of days in the time profile pattern
         * 
         * @return The set of days in the time profile pattern
         */
        public Set<String> getDays() {
            return days;
        }

        /**
         * Return a string representing a description of the time profile
         * 
         * @return A string representing a description of the time profile
         */
        public String getDescription() {
            StringBuilder builder = new StringBuilder();

            if (period == Period.DAY) {
                builder.append(repeat).append("d");
            } 
            else if (period == Period.WEEK) {
                for (String d : days)
                    builder.append(d);
            }
            builder.append(" ")
                   .append(getStartTimeStr())
                   .append("-")
                   .append(getEndTimeStr())
                   .append(", req = ")
                   .append(getReqTimeStr())
                   .append(" (")
                   .append(getStartDateStr())
                   .append(" -> ")
                   .append(getEndDateStr())
                   .append(")");

            return builder.toString();
        }

        /**
         * Return a display string for the time profile
         * 
         * @return A display string for the time profile
         */
        public String getDisplayName() {
            return name + " (" + getDescription() + ")";
        }

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Setter Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Set the name of the time profile
         * 
         * @param name The name of the time profile
         */
        public void setName(String name) {
            this.name = name;
        }

        /**
         * Set the start date of the time profile
         * 
         * @param date The start date
         */
        public void setStartDate(String date) {
            startDate = LocalDate.parse(date, D_FORMATTER);
        }

        /**
         * Set the end date of the time profile
         * 
         * @param date The end date
         */
        public void setEndDate(String date) {
            endDate = LocalDate.parse(date, D_FORMATTER);
        }

        /**
         * Set the start time of the time profile
         * 
         * @param time The start time
         */
        public void setStartTime(String time) {
            startTime = LocalTime.parse(time, T_LAZY_FORMATTER);
        }

        /**
         * Set the end time of the time profile
         * 
         * @param time The end time
         */
        public void setEndTime(String time) {
            endTime = LocalTime.parse(time, T_LAZY_FORMATTER);
        }

        /**
         * Set the req time of the time profile
         * 
         * @param time The req time
         */
        public void setReqTime(String time) {
            reqTime = LocalTime.parse(time, T_LAZY_FORMATTER);
        }

        /**
         * Set the start time stdev of the time profile
         * 
         * @param time The start time stdev
         */
        public void setStartStdev(String time) {
            startStdev = LocalTime.parse(time, T_LAZY_FORMATTER);
        }

        /**
         * Set the end time stdev of the time profile
         * 
         * @param time The end time stdev
         */
        public void setEndStdev(String time) {
            endStdev = LocalTime.parse(time, T_LAZY_FORMATTER);
        }

        /**
         * Set the req time stdev of the time profile
         * 
         * @param time The req time stdev
         */
        public void setReqStdev(String time) {
            reqStdev = LocalTime.parse(time, T_LAZY_FORMATTER);
        }

        /**
         * Set the repeat frequency of the time profile
         * 
         * @param r The repeat frequency
         */
        public void setRepeat(Integer r) {
            this.repeat = r;
        }

        /**
         * Set the period of the time profile
         * 
         * @param period The time profile period
         */
        public void setPeriod(String period) {
            switch (period) {
            case "day":
                this.period = Period.DAY;
                break;
            case "week":
                this.period = Period.WEEK;
                break;
            default:
                throw new IllegalArgumentException("Invalid period " + period);
            }
        }

        /**
         * Add a day to the time profile pattern
         * 
         * @param day The day to add
         */
        public void addDay(String day) {
            days.add(day);
        }

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Other Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Return true if the time profile is valid
         * 
         * @return True if the time profile is valid
         */
        public boolean isValid() {
            return name != null && 
                    startDate != null && endDate != null && 
                    startTime != null && startStdev != null && 
                    endTime != null && endStdev != null && 
                    reqTime != null && reqStdev != null && 
                    period != null;
        }

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Iterators / Streams
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return an iterator over the time profiles
     * 
     * @return An iterator over the time profiles
     */
    @Override
    public Iterator<TimeProfile> iterator() {
        return profiles.values().iterator();
    }

    /**
     * Return a stream over the time profiles
     * 
     * @return A stream over the time profiles
     */
    public Stream<TimeProfile> stream() {
        return profiles.values().stream();
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Queries / Access Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return the time profile with the given name
     * 
     * @param name The name of the time profile
     * @return The time profile with the given name
     */
    public TimeProfile get(String name) {
        return profiles.get(name);
    }

    /**
     * Return the name of the time profile with the given description
     * 
     * @param desc The description of the time profile
     * @return The name of the time profile with the given description
     */
    public String getName(String desc) {
        return profilesByDescription.get(desc);
    }

    /**
     * Add a time profile to the model
     * 
     * @param tp The time profile to add
     */
    public void addTimeProfile(TimeProfile tp) {
        profiles.put(tp.name, tp);
        profilesByDescription.put(tp.getDescription(), tp.getName());
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Other Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return true if the time profile is valid
     * 
     * @return True if the time profile is valid
     */
    public boolean checkValid(boolean print) {
        ErrorStringBuilder err = new ErrorStringBuilder();
        for (TimeProfile tp : this) {
            if (!tp.isValid())
                err.append("Invalid time profile ", tp.getName());
        }

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
     * Load a JSON profile into a TimeProfile object
     * 
     * @param profile The JSONArray to load
     * @return A TimeProfile object derived from the JSON profile
     */
    public static TimeProfile loadJSON(JSONArray profile) {
        TimeProfilesModel tpm = TimeProfilesModel.getInstance();
        TimeProfile       tp  = tpm.new TimeProfile();

        JSONObject patdur = (JSONObject) profile.get(0);
        JSONObject pat    = (JSONObject) patdur.get("pattern");
        tp.setStartDate((String) pat.get("start-date"));
        tp.setEndDate((String) pat.get("end-date"));

        tp.setPeriod((String) pat.get("period"));
        JSONObject details = (JSONObject) pat.get("period-details");
        if (details.containsKey("repeat-every"))
            tp.setRepeat((int) (long) details.get("repeat-every"));
        if (details.containsKey("days-of-week")) {
            JSONArray days = (JSONArray) details.get("days-of-week");
            for (Object day : days) {
                switch ((int) (long) day) {
                case 1: tp.addDay("M"); break;
                case 2: tp.addDay("Tu");  break;
                case 3: tp.addDay("W"); break;
                case 4: tp.addDay("Th");  break;
                case 5: tp.addDay("F"); break;
                case 6: tp.addDay("Sa");  break;
                case 7: tp.addDay("Su"); break;
                }
            }
        }

        JSONObject dur   = (JSONObject) patdur.get("duration");
        JSONArray  start = (JSONArray) dur.get("start-time");
        tp.setStartTime((String) start.get(0));
        tp.setStartStdev((String) start.get(1));
        JSONArray end = (JSONArray) dur.get("end-time");
        tp.setEndTime((String) end.get(0));
        tp.setEndStdev((String) end.get(1));
        JSONArray req = (JSONArray) dur.get("required");
        tp.setReqTime((String) req.get(0));
        tp.setReqStdev((String) req.get(1));

        tp.setName(tpm.getName(tp.getDescription()));

        return tp;
    }

    /**
     * Write the TimeProfile object into a JSONArray
     * 
     * @param tp The time profile to write
     * @return A JSONArray representing the TimeProfile object
     */
    @SuppressWarnings("unchecked")
    public static JSONArray writeJSON(TimeProfile tp) {
        JSONObject pat = new JSONObject();
        pat.put("start-date", tp.getStartDateStr());
        pat.put("end-date", tp.getEndDateStr());
        pat.put("period", tp.getPeriod());
        JSONObject details = new JSONObject();
        if (tp.getRepeat() != 1)
            details.put("repeat-every", tp.getRepeat());
        if (tp.period == TimeProfile.Period.WEEK) {
            JSONArray days = new JSONArray();
            for (String d : tp.getDays()) {
                switch (d) {
                case "M":  days.add(1); break;
                case "Tu": days.add(2); break;
                case "W":  days.add(3); break;
                case "Th": days.add(4); break;
                case "F":  days.add(5); break;
                case "Sa": days.add(6); break;
                case "Su": days.add(7); break;
                }
            }
            details.put("days-of-week", days);
        }
        pat.put("period-details", details);

        JSONObject dur   = new JSONObject();
        JSONArray  sTime = new JSONArray();
        sTime.add(tp.getStartTimeStr()+":00");
        sTime.add(tp.getStartStdevStr()+":00");
        JSONArray eTime = new JSONArray();
        eTime.add(tp.getEndTimeStr()+":00");
        eTime.add(tp.getEndStdevStr()+":00");
        JSONArray rTime = new JSONArray();
        rTime.add(tp.getReqTimeStr()+":00");
        rTime.add(tp.getReqStdevStr()+":00");
        dur.put("start-time", sTime);
        dur.put("end-time", eTime);
        dur.put("required", rTime);

        JSONObject patdur = new JSONObject();
        patdur.put("pattern", pat);
        patdur.put("duration", dur);

        JSONArray profile = new JSONArray();
        profile.add(patdur);

        return profile;
    }

    /**
     * Load the provided time profiles file
     * 
     * @param f The file to load
     */
    public void loadFile(File f) {
        if (f == null) {
            View.log("Error: No time profile file selected");
            return;
        }

        try {
            JSONParser parser = new JSONParser();

            JSONArray arr = (JSONArray) parser.parse(new FileReader(f));
            for (Object element : arr) {
                JSONObject o = (JSONObject) element;

                TimeProfile tp = new TimeProfile();

                // name
                tp.setName((String) o.get("name"));

                // start date
                tp.setStartDate((String) o.get("start-date"));

                // end date
                tp.setEndDate((String) o.get("end-date"));

                // start time
                tp.setStartTime((String) o.get("start-time"));

                // end time
                tp.setEndTime((String) o.get("end-time"));
                
                // req time
                tp.setReqTime((String) o.get("req-time"));

                // start stdev
                tp.setStartStdev((String) o.get("start-stdev"));

                // end stdev
                tp.setEndStdev((String) o.get("end-stdev"));

                // req stdev
                tp.setReqStdev((String) o.get("req-stdev"));

                // repeat
                tp.setRepeat((int) (long) o.get("repeat"));

                // period
                tp.setPeriod((String) o.get("period"));

                if (tp.period == TimeProfile.Period.WEEK) {
                    JSONArray days = (JSONArray) o.get("days");
                    for (Object day : days)
                        tp.addDay((String) day);
                }

                this.addTimeProfile(tp);
            }

            View.log("Loaded Time Profiles File: " + f.getAbsolutePath());

        } 
        catch (IllegalArgumentException e) {
            View.log("Error: " + e.getMessage());
        } 
        catch (ParseException e) {
            View.log("Error: Cannot parse file: " + f.getAbsolutePath());
        } 
        catch (IOException e) {
            View.log("Error: Invalid time profiles file: " + f.getAbsolutePath());
        }
    }

    /**
     * Write the time profiles to file
     */
    @SuppressWarnings("unchecked")
    public void writeToFile() {
        if (!checkValid(true))
            return;

        String fname = Model.getConfig().getBaseDir() + "TimeProfiles.json";

        JSONArray data = new JSONArray();

        for (TimeProfile tp : profiles.values()) {
            JSONObject o = new JSONObject();

            // name
            o.put("name", tp.getName());

            // start date
            o.put("start-date", tp.getStartDateStr());

            // end date
            o.put("end-date", tp.getEndDateStr());

            // start time
            o.put("start-time", tp.getStartTimeStr());

            // end time
            o.put("end-time", tp.getEndTimeStr());

            // req time
            o.put("req-time", tp.getReqTimeStr());

            // start stdev
            o.put("start-stdev", tp.getStartStdevStr());

            // end stdev
            o.put("end-stdev", tp.getEndStdevStr());

            // req stdev
            o.put("req-stdev", tp.getReqStdevStr());
            
            // period
            o.put("period", tp.getPeriod());

            // repeat
            o.put("repeat", tp.getRepeat());

            // days
            if (tp.period == TimeProfile.Period.WEEK) {
                JSONArray days = new JSONArray();
                for (String d : tp.getDays())
                    days.add(d);
                o.put("days", days);
            }

            data.add(o);
        }

        Model.getInstance().writeJSON(fname, data, "time profiles");

    }

}
