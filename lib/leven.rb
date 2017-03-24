require "leven/version"
require "leven/leven"

module Leven
  module StringDistance
    def distance(other)
      Leven.distance(self.to_s, other.to_s)
    end
  end
end
