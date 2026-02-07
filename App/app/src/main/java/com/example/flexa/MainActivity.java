package com.example.flexa;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.TextView;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class MainActivity extends AppCompatActivity {

    // UI Components
    private TextView txtPostureStatus, txtMuscleStatus, txtMuscleRaw, txtConnectionStatus;

    // Firebase Reference
    private DatabaseReference patientRef;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // 1. Initialize UI
        txtPostureStatus = findViewById(R.id.txtPostureStatus);
        txtMuscleStatus = findViewById(R.id.txtMuscleStatus);
        txtMuscleRaw = findViewById(R.id.txtMuscleRaw);
        txtConnectionStatus = findViewById(R.id.txtConnectionStatus);

        // 2. Initialize Firebase
        // NOTE: Use your exact URL
        FirebaseDatabase database = FirebaseDatabase.getInstance("https://flexa-22635-default-rtdb.firebaseio.com/");
        patientRef = database.getReference("Patient"); // Listen to "Patient" node

        // 3. Start Listener
        startMonitoring();
    }

    private void startMonitoring() {
        patientRef.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                if (snapshot.exists()) {
                    txtConnectionStatus.setText("● System Connected");
                    txtConnectionStatus.setTextColor(Color.parseColor("#4CAF50")); // Green

                    // ============================================
                    // 1. POSTURE LOGIC (SAFE vs UNSAFE)
                    // ============================================
                    String postureStr = snapshot.child("Posture").getValue(String.class);

                    if (postureStr != null) {
                        txtPostureStatus.setText(postureStr.toUpperCase());

                        if (postureStr.equalsIgnoreCase("Safe")) {
                            // Safe = GREEN
                            txtPostureStatus.setBackgroundColor(Color.parseColor("#4CAF50"));
                        } else {
                            // Unsafe = RED (Alert)
                            txtPostureStatus.setBackgroundColor(Color.parseColor("#D32F2F"));
                        }
                    }

                    // ============================================
                    // 2. MUSCLE LOGIC (RELAXED vs ACTIVE)
                    // ============================================
                    String muscleStr = snapshot.child("MuscleStatus").getValue(String.class);
                    Object rawObj = snapshot.child("MuscleRaw").getValue();

                    // Display Raw Value
                    if (rawObj != null) {
                        txtMuscleRaw.setText("Raw Sensor: " + rawObj.toString());
                    }

                    // Color Logic for Muscle Status
                    if (muscleStr != null) {
                        txtMuscleStatus.setText(muscleStr.toUpperCase());

                        if (muscleStr.equalsIgnoreCase("Relaxed")) {
                            // Relaxed = BLUE (Calm)
                            txtMuscleStatus.setBackgroundColor(Color.parseColor("#2196F3"));
                        }
                        else if (muscleStr.equalsIgnoreCase("Active")) {
                            // Active = ORANGE (Energetic/Working)
                            txtMuscleStatus.setBackgroundColor(Color.parseColor("#FF9800"));
                        }
                        else {
                            // Unknown = GREY
                            txtMuscleStatus.setBackgroundColor(Color.parseColor("#BDBDBD"));
                        }
                    }

                } else {
                    txtConnectionStatus.setText("⚠ Waiting for data...");
                }
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                txtConnectionStatus.setText("Error: " + error.getMessage());
            }
        });
    }
}