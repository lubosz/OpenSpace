/*****************************************************************************************
 *                                                                                       *
 * OpenSpace                                                                             *
 *                                                                                       *
 * Copyright (c) 2014-2020                                                               *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
 * software and associated documentation files (the "Software"), to deal in the Software *
 * without restriction, including without limitation the rights to use, copy, modify,    *
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
 * permit persons to whom the Software is furnished to do so, subject to the following   *
 * conditions:                                                                           *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all copies *
 * or substantial portions of the Software.                                              *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  *
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE  *
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                         *
 ****************************************************************************************/

#ifndef __OPENSPACE_MODULE_SPACE___RENDERABLEORBITALKEPLER___H__
#define __OPENSPACE_MODULE_SPACE___RENDERABLEORBITALKEPLER___H__

#include <openspace/rendering/renderable.h>

#include <modules/base/rendering/renderabletrail.h>
#include <modules/space/translation/keplertranslation.h>
#include <openspace/properties/stringproperty.h>
#include <openspace/properties/scalar/uintproperty.h>
#include <ghoul/glm.h>
#include <ghoul/misc/objectmanager.h>
#include <ghoul/opengl/programobject.h>

namespace openspace {

class RenderableOrbitalKepler : public Renderable {
public:
    RenderableOrbitalKepler(const ghoul::Dictionary& dictionary);

    void initializeGL() override;
    void deinitializeGL() override;

    bool isReady() const override;
    void render(const RenderData& data, RendererTasks& rendererTask) override;

    /**
        * Reads the provided data file and calls the KeplerTranslation::setKeplerElments
        * method with the correct values. If \p filename is a valid data file but contains
        * disallowed values (see KeplerTranslation::setKeplerElements), a
        * KeplerTranslation::RangeError is thrown.
        *
        * \param filename The path to the file that contains the data file.
        *
        * \throw ghoul::RuntimeError if the data file does not exist or there is a
        *        problem with its format.
        * \pre The \p filename must exist
        */
    virtual void readDataFile(const std::string& filename) = 0;

protected:
    int countDays(int year);
    int countLeapSeconds(int year, int dayOfYear);
    double calculateSemiMajorAxis(double meanMotion);
    double epochFromSubstring(const std::string& epochString);
    double epochFromYMDdSubstring(const std::string& epochString);
    int daysIntoGivenYear(int month, int dayOfMonth);
    std::function<void()> reinitializeTrailBuffers;

    const std::vector<int> LeapYears = {
        1956, 1960, 1964, 1968, 1972, 1976, 1980, 1984, 1988, 1992, 1996,
        2000, 2004, 2008, 2012, 2016, 2020, 2024, 2028, 2032, 2036, 2040,
        2044, 2048, 2052, 2056
    };
    const std::vector<int> DaysOfMonths = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    enum Months {
        January = 0,
        February,
        March,
        April,
        May,
        June,
        July,
        August,
        September,
        October,
        November,
        December
    };
    struct KeplerParameters {
        double inclination = 0.0;
        double semiMajorAxis = 0.0;
        double ascendingNode = 0.0;
        double eccentricity = 0.0;
        double argumentOfPeriapsis = 0.0;
        double meanAnomaly = 0.0;
        double meanMotion = 0.0;
        double epoch = 0.0;
        double period = 0.0;
    };
    const double convertAuToKm = 1.496e8;
    const double convertDaysToSecs = 86400.;
    std::vector<KeplerParameters> _data;
    properties::UIntProperty _upperLimit;
    properties::Property::OnChangeHandle _upperLimitCallbackHandle;

private:
    struct Vertex {
        glm::vec3 position = glm::vec3(0.f);
        glm::vec3 color = glm::vec3(0.f);
        glm::vec2 texcoord = glm::vec2(0.f);
    };

    /// The layout of the VBOs
    struct TrailVBOLayout {
        float x = 0.f;
        float y = 0.f;
        float z = 0.f;
        float time = 0.f;
        double epoch = 0.0;
        double period = 0.0;
    };

    KeplerTranslation _keplerTranslator;

    /// The backend storage for the vertex buffer object containing all points for this
    /// trail.
    std::vector<TrailVBOLayout> _vertexBufferData;

    GLuint _vertexArray;
    GLuint _vertexBuffer;
    GLuint _indexBuffer;

    //GLuint _vaoTest; // vertexArrayObject
    //GLuint _vboTest; // vertextBufferObject
    //GLuint _eboTest; // elementBufferObject/ indexBufferObject   

    void updateBuffers();

    ghoul::opengl::ProgramObject* _programObject;
    properties::StringProperty _path;
    properties::UIntProperty _nSegments;
    RenderableTrail::Appearance _appearance;
    glm::vec3 _position = glm::vec3(0.f);

    UniformCache(modelView, projection, lineFade, inGameTime, color, opacity,
        numberOfSegments) _uniformCache;
};

} // namespace openspace

#endif // __OPENSPACE_MODULE_SPACE___RENDERABLEORBITALKEPLER___H__
