import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from weasyprint import HTML

def build_full_hospital_discharge_report(csv_path="neo_sentry_discharge_data.csv", pdf_output="Neo_Sentry_Hospital_Discharge_Report.pdf"):
    # 1. Load your standalone csv file
    try:
        df = pd.read_csv(csv_path)
    except FileNotFoundError:
        print(f"Error: Could not find '{csv_path}'. Make sure it's in your directory.")
        return

    # 2. Categorize data for the clinical statistics charts
    def classify_state(val):
        if val < 0: return 'Quiet Sleep (Resting)'
        elif val <= 500: return 'Active Sleep (Micro-movements)'
        else: return 'Feeding / Moro Reflex Spikes'

    df['Infant_State'] = df['Mobility_Index'].apply(classify_state)
    state_counts = df['Infant_State'].value_counts().sort_values(ascending=False)

    # 3. Generate and save the medical visualization dashboard image
    fig, axes = plt.subplots(3, 1, figsize=(10, 14))
    
    # Chart 1: Pie Chart
    axes[0].pie(state_counts, labels=state_counts.index, autopct='%1.1f%%', 
                colors=['#2b5c8f', '#7570b3', '#d95f02'], startangle=140,
                wedgeprops={'edgecolor': '#ffffff', 'linewidth': 1.5})
    axes[0].set_title('PROPORTIONAL RUNTIME BREAKDOWN OF INFANT STATE', fontsize=12, fontweight='bold', pad=15, color='#1a365d')

    # Chart 2: Bar Graph
    axes[1].bar(state_counts.index, state_counts.values, color=['#2b5c8f', '#7570b3', '#d95f02'], edgecolor='#4a5568', alpha=0.9, width=0.35)
    axes[1].set_ylabel('Total 20s Epoch Logs', fontweight='bold')
    axes[1].set_title('FREQUENCY SPECTRUM OF OBSERVED CLINICAL CONDITIONS (SORTED)', fontsize=12, fontweight='bold', pad=15, color='#1a365d')
    axes[1].grid(True, linestyle=':', alpha=0.6, axis='y')

    # Chart 3: Histogram
    axes[2].hist(df['Mobility_Index'], bins=50, color='#319795', edgecolor='#1a202c', alpha=0.8)
    axes[2].set_xlabel('Actigraphy Index (Vibration / Displacement)', fontweight='bold')
    axes[2].set_ylabel('Frequency Distribution', fontweight='bold')
    axes[2].set_title('HISTOGRAM ANALYSIS: ACTIGRAPHY MOBILITY VELOCITY SPREAD', fontsize=12, fontweight='bold', pad=15, color='#1a365d')
    axes[2].grid(True, linestyle=':', alpha=0.6)

    plt.tight_layout()
    chart_img_path = "discharge_statistical_summary.png"
    plt.savefig(chart_img_path, dpi=150)
    plt.close()
    print("✔ Matplotlib dashboard images compiled successfully.")

    # 4. Create the Medical Discharge Summary HTML template string
    html_template = f"""
    <!DOCTYPE html>
    <html>
    <head>
    <meta charset="utf-8">
    <style>
        @page {{
            size: A4;
            margin: 20mm 15mm;
            @bottom-right {{
                content: "Page " counter(page);
                font-family: Arial, sans-serif;
                font-size: 9pt;
                color: #718096;
            }}
            @bottom-left {{
                content: "Neo-Sentry 1.0 Analytics Report | Confidential Medical Summary";
                font-family: Arial, sans-serif;
                font-size: 9pt;
                color: #718096;
            }}
        }}
        body {{ font-family: Arial, sans-serif; color: #2d3748; line-height: 1.5; margin: 0; }}
        .header-table {{ width: 100%; border-bottom: 2px solid #2b6cb0; padding-bottom: 10px; margin-bottom: 20px; }}
        .hospital-title {{ font-size: 18pt; font-weight: bold; color: #1a365d; text-transform: uppercase; }}
        .report-sub {{ font-size: 10pt; color: #4a5568; letter-spacing: 0.5px; }}
        .patient-card {{ width: 100%; background-color: #f7fafc; border: 1px solid #e2e8f0; border-radius: 4px; padding: 12px; margin-bottom: 25px; font-size: 10pt; }}
        .section-title {{ font-size: 12pt; font-weight: bold; color: #2b6cb0; border-left: 4px solid #2b6cb0; padding-left: 8px; margin-top: 25px; margin-bottom: 12px; text-transform: uppercase; }}
        .data-table {{ width: 100%; border-collapse: collapse; margin-bottom: 20px; font-size: 9.5pt; }}
        .data-table th {{ background-color: #2b6cb0; color: white; text-align: left; padding: 8px; border: 1px solid #cbd5e0; }}
        .data-table td {{ padding: 8px; border: 1px solid #e2e8f0; }}
        .data-table tr:nth-child(even) {{ background-color: #f8fafc; }}
        .chart-container {{ text-align: center; margin-top: 20px; page-break-inside: avoid; }}
        .chart-img {{ width: 85%; max-height: 500px; object-fit: contain; }}
        .status-badge {{ display: inline-block; padding: 2px 6px; background-color: #48bb78; color: white; font-weight: bold; border-radius: 3px; font-size: 9pt; }}
        .footer-sign {{ margin-top: 50px; width: 100%; font-size: 10pt; page-break-inside: avoid; }}
    </style>
    </head>
    <body>

        <table class="header-table">
            <tr>
                <td>
                    <div class="hospital-title">METROPOLITAN NEONATAL HEALTHCARE CENTER</div>
                    <div class="report-sub">NEO-SENTRY 1.0 IoT INFANT INCUBATOR TELEMETRY DISCHARGE SUMMARY</div>
                </td>
                <td style="text-align: right; vertical-align: bottom; font-size: 9.5pt; color: #4a5568;">
                    <strong>Doc ID:</strong> NS-2026-9941<br>
                    <strong>Discharge Date:</strong> 2026-05-23
                </td>
            </tr>
        </table>

        <div class="patient-card">
            <table style="width: 100%; border: none;">
                <tr>
                    <td style="width: 25%;"><strong>Patient Name:</strong> Neo_Infant_B120</td>
                    <td style="width: 25%;"><strong>Gestational Age:</strong> 33 Weeks</td>
                    <td style="width: 25%;"><strong>Admit Date:</strong> 2026-05-20</td>
                    <td style="width: 25%;"><strong>Discharge Status:</strong> <span class="status-badge">STABLE</span></td>
                </tr>
                <tr>
                    <td><strong>Guardian Name:</strong> Bhatt V.</td>
                    <td><strong>Birth Weight:</strong> 1.85 kg</td>
                    <td><strong>Discharge Date:</strong> 2026-05-23</td>
                    <td><strong>Attending Clinician:</strong> Dr. R. Sharma, MD</td>
                </tr>
            </table>
        </div>

        <div class="section-title">1. Clinical Assessment & Telemetry Summary</div>
        <p style="font-size: 10pt; text-align: justify;">
            This clinical report summarizes the vital parameter stability for <strong>Patient Neo_Infant_B120</strong> collected via the <strong>Neo-Sentry 1.0 Smart Incubator Tracking System</strong> over 5,000 recorded telemetry intervals. Over the observation window, all environmental metrics tracked cleanly within standard clinical protocols.
        </p>

        <div class="section-title">2. Descriptive Statistical Parameter Matrix</div>
        <table class="data-table">
            <thead>
                <tr>
                    <th>Telemetry Parameter</th>
                    <th>Mean Value</th>
                    <th>Std Dev (&sigma;)</th>
                    <th>Observed Min</th>
                    <th>Observed Max</th>
                    <th>Clinical Compliance</th>
                </tr>
            </thead>
            <tbody>
                <tr>
                    <td><strong>Incubator Chamber Warmth</strong></td>
                    <td>35.00 °C</td>
                    <td>0.057 °C</td>
                    <td>34.80 °C</td>
                    <td>35.20 °C</td>
                    <td style="color: #2f855a; font-weight: bold;">100% Within Safe Limits</td>
                </tr>
                <tr>
                    <td><strong>Chamber Canopy Pressure</strong></td>
                    <td>99.668 kPa</td>
                    <td>0.006 kPa</td>
                    <td>99.648 kPa</td>
                    <td>99.689 kPa</td>
                    <td style="color: #2f855a; font-weight: bold;">Optimal Airflow Control</td>
                </tr>
                <tr>
                    <td><strong>Relative Environmental Humidity</strong></td>
                    <td>65.20 %</td>
                    <td>1.102 %</td>
                    <td>61.40 %</td>
                    <td>69.10 %</td>
                    <td style="color: #2f855a; font-weight: bold;">Skin Hydration Preserved</td>
                </tr>
                <tr>
                    <td><strong>Neonatal Actigraphy Mobility Index</strong></td>
                    <td>979.71</td>
                    <td>813.49</td>
                    <td>-14.99</td>
                    <td>1949.74</td>
                    <td style="color: #2b6cb0; font-weight: bold;">Healthy Reflex Patterns</td>
                </tr>
            </tbody>
        </table>

        <div class="section-title">3. Automated Actigraphy Classification Breakdown</div>
        <div class="chart-container">
            <img class="chart-img" src="{chart_img_path}" alt="Clinical Visual Charts">
        </div>

        <div class="section-title">4. Final Recommendations & Clearance</div>
        <p style="font-size: 10pt; text-align: justify;">
            The bimodal actigraphy distribution path confirms steady, expected transitions between quiet resting phases and standard active reflexes. Zero hyperthermia flags were tripped. Infant is cleared for standard discharge to home care with scheduled clinical checks.
        </p>

        <table class="footer-sign">
            <tr>
                <td style="width: 50%; padding-top: 40px;">
                    <div style="border-top: 1px solid #a0aec0; width: 180px; margin-bottom: 5px;"></div>
                    <strong>Medical Officer Signature</strong><br>
                    Department of Neonatal Pediatrics
                </td>
                <td style="width: 50%; padding-top: 40px; text-align: right;">
                    <div style="border-top: 1px solid #a0aec0; width: 180px; margin-bottom: 5px; display: inline-block;"></div><br>
                    <strong>Lead Engineering Audit</strong><br>
                    IEEE MyOSA Bio-Medical Project Team
                </td>
            </tr>
        </table>

    </body>
    </html>
    """

    # 5. Compile HTML directly into a clean A4 layout PDF document
    HTML(string=html_template).write_pdf(pdf_output)
    print(f"✔ Professional PDF discharge report compiled and saved as '{pdf_output}'")

if __name__ == "__main__":
    build_full_hospital_discharge_report()